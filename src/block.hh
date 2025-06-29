#ifndef HILL__BLOCK_HH_INCLUDED
#define HILL__BLOCK_HH_INCLUDED

#include "val_ref.hh"

#include "exceptions.hh"
#include "instr.hh"

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace hill {

	struct scope { // The names and values
		std::map<std::string, std::vector<val_ref>> ids;
		frame_def frame;

		std::shared_ptr<scope> parent = nullptr;

		const std::vector<val_ref> *find_id(const std::string &identifier) const
		{
			if (ids.contains(identifier)) {
				return &ids.at(identifier);
			} else if (parent) {
				return parent->find_id(identifier);
			} else {
				return nullptr;
			}
		}

		const val_ref *find_val_ref(const std::string &identifier) const
		{
			auto v = find_id(identifier);

			return v && v->size()>0 ? &((*v)[0]) : nullptr;
		}

		bool are_matching(const type &pattern, const type &actual) const
		{
			if (pattern==actual) return true;
			else return false;
		}

		const val_ref *find_matching_val_ref(const std::string &identifier, const type &pattern) const
		{
			const val_ref *r = nullptr;

			if (ids.contains(identifier)) {
				for (const auto &val_ref: ids.at(identifier)) {
					if (pattern.matches(val_ref.type)) {
						return &val_ref;
					}
				}
			}

			return r ? r : (parent ? parent->find_matching_val_ref(identifier, pattern) : nullptr);
		}

		static std::shared_ptr<scope> create()
		{
			return std::make_shared<scope>();
		}
		static std::shared_ptr<scope> create(const std::shared_ptr<scope> &parent)
		{
			auto s = std::make_shared<scope>();
			s->parent = parent;
			return s;
		}
	};

	struct type_stack {
		void push(const type &ts)
		{
			types.push_back(ts);
		}

		const type &top(size_t pos=0)
		{
			if (types.size() < (1+pos)) throw internal_exception();
			return types[types.size()-1-pos];
		}

		type &vtop(size_t pos=0)
		{
			if (types.size() < (1+pos)) throw internal_exception();
			return types[types.size()-1-pos];
		}

		void pop()
		{
			types.pop_back();
		}

	private:
		std::vector<type> types;
	};

	inline instr make_val_instr(const val_ref &val, int offset)
	{
		if (val.mt == mem_type::STACK) {
			return instr {
				.op = op_code::LOAD,
				.res_type = val.type,
				.val = {.ix=val.ix},
				.arg1_type = type(),
				.arg2_type = type(),
				.offset = offset};
		} else if (val.mt == mem_type::LITERAL) {
			if (val.type.first()==basic_type::FUNC) {
				return make_instr(op_code::LOADI, val.type, val.p, offset);
			} else {
				return make_instr(op_code::LOADI, val.type, val.u32, offset);
			}
		} else {
			throw internal_exception();
		}
	}

	inline bool resolve_rs_id_vals(type_stack &ts, std::vector<instr> &instrs, const scope &s)
	{
		auto &rsts = ts.vtop();
		if (rsts.types.empty()) {
			auto &rsinstr = instrs[rsts.iref];
			const val_ref *val = s.find_val_ref(rsinstr.id);
			if (!val) return false;

			rsinstr = make_val_instr(*val, rsinstr.offset);

			rsts = val->type;
		}

		return true;
	}

	inline bool resolve_ls_id_vals(type_stack &ts, std::vector<instr> &instrs, const scope &s)
	{
		auto &lsts = ts.vtop(1);
		if (lsts.types.empty()) {
			auto &lsinstr = instrs[lsts.iref];
			const val_ref *val = s.find_val_ref(lsinstr.id);
			if (!val) return false;

			lsinstr = make_val_instr(*val, lsinstr.offset);

			lsts = val->type;
		}

		return true;
	}

	inline bool resolve_call_id_vals(type_stack &ts, std::vector<instr> &instrs, const scope &s)
	{
		if (!resolve_rs_id_vals(ts, instrs, s)) return false;

		auto &lsts = ts.vtop(1);
		if (lsts.types.empty()) {
			// TODO: Make ts: FUNC wildcard RS
			auto &lsinstr = instrs[lsts.iref];

			type type(basic_type::FUNC);
			type.types.push_back(basic_type::UNDECIDED);
			type.types.insert(type.types.end(), ts.top().types.cbegin(), ts.top().types.cend());
			type.types.push_back(basic_type::END);

			const val_ref *val = s.find_matching_val_ref(lsinstr.id, type);
			if (!val) return false;

			lsinstr = make_val_instr(*val, lsinstr.offset);

			//auto &lsts = ts.vtop();
			lsts = val->type;
		}

		return true;
	}

	inline bool resolve_var_id_vals(type_stack &ts, std::vector<instr> &instrs, const scope &s)
	{
		if (!resolve_rs_id_vals(ts, instrs, s)) return false;

		auto &lsts = ts.vtop(1);
		if (lsts.types.empty()) {
			auto lsinstr = instrs[lsts.iref];

			const val_ref *val = s.find_val_ref(lsinstr.id);
			if (!val) return false;

			lsinstr = make_val_instr(*val, lsinstr.offset);

			auto &rsts = ts.vtop();
			rsts = val->type;
		}

		return true;
	}

	struct block { // The code
		scope s;
		type_stack ts;

		literal_values values;
		std::vector<instr> instrs;

		std::string to_str() const
		{
			std::stringstream ss;
			for (auto &instr : instrs) {
				ss << instr.to_str() << '\n';
			}
			ss << values.to_str();
			return ss.str();
		}

		void add(const token &t)
		{
			switch (t.get_type()) {
			case tt::RPAR:
				ts.vtop().close_tuple();
				break;
			case tt::RCURLY:
				break;
			case tt::RSQUARE:
				{
					auto &ttype = ts.vtop();
					std::vector<basic_type> itype;

					int cnt = 0;
					if (ttype.types[0]==basic_type::TUPLE) {
						itype = inner_type(ttype.types, 1);
						for (size_t ix=1; ttype.types[ix]!=basic_type::END; ix+=itype.size()) {
							if (itype!=inner_type(ttype.types, ix))
								throw semantic_error_exception(error_code::ARRAY_ELM_TYPE_MISMATCH);
							++cnt;
						}
					} else {
						itype = ttype.types;
						cnt = 1;
					}

					ttype.types.clear();
					ttype.types.push_back(basic_type::ARRAY);
					ttype.types.insert(ttype.types.end(), itype.begin(), itype.end() );
					ttype.types.push_back((basic_type)cnt);

					ttype.types.push_back(basic_type::END);
				}
				break;
			case tt::END:
				{
					if (!resolve_rs_id_vals(ts, instrs, s)) throw semantic_error_exception(error_code::UNDEFINED_ID);

					type res_type = ts.top();
					instrs.push_back(instr{
						.op = op_code::END,
						.res_type = res_type,
						.val = {},
						.arg1_type = type(),
						.arg2_type = type()});
					res_type.iref = instrs.size()-1;
					ts.push(res_type);
				}
				break;
			case tt::NAME:
				{
					auto instr = make_placeholder_instr(t.text, 0);
					instrs.push_back(instr);
					//auto t = type_spec {
					auto res_type = type();
					res_type.iref = instrs.size()-1;
					ts.push(res_type);
				}
				break;
			case tt::NUM:
				{
					char *endp = nullptr;
#define USE_LOAD_IMMEDIATE
					if (t.str().find('.')!=std::string::npos) { // floating point (just  double for now)
						type res_type = type(basic_type::F64);
#ifdef USE_LOAD_IMMEDIATE
						instrs.push_back(instr{
							.op = op_code::LOADI,
							.res_type = res_type,
							.val = {.imm_f64 = static_cast<double>(std::strtold(t.get_text().c_str(), &endp))},
							.arg1_type = type(),
							.arg2_type = type()});
#else
						auto vix = values.add(std::strtold(t.get_text().c_str(), &endp));
						instrs.push_back(instr{
							.op = op_code::LOAD,
							.res_type = res_type,
							.val = {.ix = vix}});
#endif /* USE_LOAD_IMMEDIATE */
						res_type.iref = instrs.size()-1;
						ts.push(res_type);
					} else { // integral
						type res_type = type(basic_type::I32);
#ifdef USE_LOAD_IMMEDIATE
						instrs.push_back(instr{
							.op = op_code::LOADI,
							.res_type = res_type,
							.val = {.imm_i32 = static_cast<int32_t>(std::strtol(t.get_text().c_str(), &endp, 10))},
							.arg1_type = type(),
							.arg2_type = type()});
#else
						auto vix = values.add(std::strtoll(t.get_text().c_str(), &endp, 10));
						instrs.push_back(instr{
							.op = op_code::LOAD,
							.res_type = res_type,
							.val = {.ix = vix}});
#endif /* USE_LOAD_IMMEDIATE */
						res_type.iref = instrs.size()-1;
						ts.push(res_type);
					}
				}
				break;
			case tt::OP_PLUS:
				{
					/*const auto res_type = convert_binary(
						t.get_type(),
						second_last().res_dt,
						last().res_dt);*/

					if (!resolve_var_id_vals(ts, instrs, s))
						throw semantic_error_exception(error_code::UNDEFINED_ID);

					type res_type = ts.top();
					instrs.push_back(instr{
						.op = op_code::ADD,
						.res_type = res_type,
						.val = {},
						.arg1_type = ts.top(1),
						.arg2_type = ts.top()});
					ts.pop();
					ts.pop();
					res_type.iref = instrs.size()-1;
					ts.push(res_type);
				}
				break;
			case tt::OP_MINUS:
				{
					/*const auto res_type = convert_binary(
						t.get_type(),
						second_last().res_dt,
						last().res_dt);*/

					if (t.get_arity()==tt_arity::LUNARY) {
						if (!resolve_rs_id_vals(ts, instrs, s))
							throw semantic_error_exception(error_code::UNDEFINED_ID);

						type res_type = ts.top();
						instrs.push_back(instr{
							.op = op_code::NEG,
							.res_type = res_type,
							.val = {},
							.arg1_type = ts.top(),
							.arg2_type = type()});
						ts.pop();
						res_type.iref = instrs.size()-1;
						ts.push(res_type);
					} else if (t.get_arity()==tt_arity::BINARY) {
						if (!resolve_var_id_vals(ts, instrs, s))
							throw semantic_error_exception(error_code::UNDEFINED_ID);

						type res_type = ts.top();
						instrs.push_back(instr{
							.op = op_code::SUB,
							.res_type = res_type,
							.val = {},
							.arg1_type = ts.top(1),
							.arg2_type = ts.top()});
						ts.pop();
						ts.pop();
						res_type.iref = instrs.size()-1;
						ts.push(res_type);
					} else {
						throw internal_exception();
					}
				}
				break;
			case tt::OP_STAR:
				{
					if (!resolve_var_id_vals(ts, instrs, s))
						throw semantic_error_exception(error_code::UNDEFINED_ID);

					type res_type = ts.top();
					instrs.push_back(instr{
						.op = op_code::MUL,
						.res_type = res_type,
						.val = {},
						.arg1_type = ts.top(1),
						.arg2_type = ts.top()});
					ts.pop();
					ts.pop();
					res_type.iref = instrs.size()-1;
					ts.push(res_type);
				}
				break;
			case tt::OP_COLON_EQ:
				{
					// TODO: Check if id with same type already exists in this scope.
					// If it does, that is a compilation error
					// Maybe try to create some nice error message?

					/*const auto res_type = convert_binary(
						t.get_type(),
						second_last().res_dt,
						last().res_dt);*/

					if (!resolve_rs_id_vals(ts, instrs, s))
						throw semantic_error_exception(error_code::UNDEFINED_ID);

					type res_type = ts.top();

					// "Copy" value to stack
					// Bind id instruction on left side to the value
					auto val = val_ref(
						mem_type::STACK,
						s.frame.add(res_type.mem_size(), 1),
						res_type);

					if (instrs.size()<2 || instrs[instrs.size()-2].op!=op_code::ID)
						throw semantic_error_exception(error_code::UNDEFINED_ID);
					s.ids[instrs[instrs.size()-2].id].push_back(val);

					// TODO: Optimization: Do not copy if immutable variable and right side is immutable

					// Create load value to stack instruction
					instrs.push_back(instr{
						.op = op_code::COPY,
						.res_type = val.type,
						.val = {.ix=val.ix},
						.arg1_type = type(),
						.arg2_type = ts.top()});
					ts.pop();
					res_type.iref = instrs.size()-1;
					ts.push(res_type);
				}
				break;
#if 0
			case tt::OP_EQ:
				{
					// TODO: Check parent scope(s)
					// If identifier starts with '@'. Only check root scope

					if (!resolve_var_id_vals(ts, instrs, s)) throw semantic_error_exception();

					type_spec res_type = ts.top();

					auto val = s.find_val_ref(this->curr_id);
					if (!val) {throw semantic_error_exception();}
					if (val->ts != res_type) {throw semantic_error_exception();}
					if (val->mt != mem_type::STACK) {throw semantic_error_exception();}
					// TODO: Check mutability of 'val'

					instrs.push_back(instr{
						.op = op_code::COPY,
						.res_type = res_type,
						.val = {.ix=val->ix},
						.arg1_type = type(),
						.arg2_type = ts.top()});
					ts.pop();
					ts.push(res_type);
				}
				break;
#endif
			case tt::OP_COMMA:
				{
					/*const auto res_type = convert_binary(
						t.get_type(),
						second_last().res_dt,
						last().res_dt);*/

					if (!resolve_var_id_vals(ts, instrs, s)) 
						throw semantic_error_exception(error_code::UNDEFINED_ID);

					type res_type=build_tuple(ts.top(1), ts.top());

					instrs.push_back(instr{
						.op = op_code::TUPLE,
						.res_type = res_type,
						.val = {},
						.arg1_type = type(),
						.arg2_type = type()});

					ts.pop();
					ts.pop();
					res_type.iref = instrs.size()-1;
					ts.push(res_type);
				}
				break;
			case tt::CALL:
				{
					if (!resolve_call_id_vals(ts, instrs, s)) 
						throw semantic_error_exception(error_code::UNDEFINED_ID);

					if (ts.top(1).is_pipe_arg) {
						// TODO: join top(1) and top() as a tuple on ts
						auto l_type = ts.top(1);
						auto r_type = ts.top();

						ts.pop();
						ts.pop();
						ts.push(build_tuple(l_type, r_type));

						if (!resolve_ls_id_vals(ts, instrs, s)) 
							throw semantic_error_exception(error_code::UNDEFINED_ID);
					}

					type res_type;
					type arg_type;
					if (ts.top(1).first()==basic_type::FUNC) {
						res_type = ts.top(1).inner_type(1);
						arg_type = ts.top(1).inner_type(1+res_type.types.size());
						
						if (arg_type!=ts.top()) 
							throw semantic_error_exception(error_code::UNDEFINED_ID);

						instrs.push_back(instr{
							.op = op_code::CALL,
							.res_type = res_type,
							.val = {},
							.arg1_type = ts.top(1),
							.arg2_type = ts.top()});
					} else {
						throw semantic_error_exception(error_code::CALLING_NO_FUNC);
					}

					ts.pop();
					ts.pop();
					res_type.iref = instrs.size()-1;
					ts.push(res_type);
				}
				break;
			case tt::OP_DOT: // Dot defaults to piping
			case tt::OP_OR_GREATER: // Piping
				{
					if (!resolve_ls_id_vals(ts, instrs, s)) 
						throw semantic_error_exception(error_code::UNDEFINED_ID);

					// things.where {_/2==0} . select {_*9} . last ();
					// last (select (where (things, ({_/2==0}), {_*9}), ());

					auto arg1_type = ts.top(1);
					auto pfunc_type = ts.top();

					// Add padding to place function first end partial data next
					instrs[arg1_type.iref].offset = (int)type(basic_type::FUNC).mem_size();
					instrs[pfunc_type.iref].offset = -((int)arg1_type.mem_size() + (int)type(basic_type::FUNC).mem_size());

					arg1_type.is_pipe_arg = true;

					ts.pop();
					ts.pop();
					ts.push(pfunc_type);
					ts.push(arg1_type);
				}
				break;
			default:
				throw not_implemented_exception();
			}
		}
	};
}

#endif /* HILL__BLOCK_HH_INCLUDED */
