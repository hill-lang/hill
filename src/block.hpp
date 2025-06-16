#ifndef HILL__BLOCK_HPP_INCLUDED
#define HILL__BLOCK_HPP_INCLUDED

#include "val_ref.hpp"

#include "exceptions.hpp"
#include "instr.hpp"

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

		bool are_matching(const type_spec &pattern, const type_spec &actual) const
		{
			if (pattern==actual) return true;


		}

		const val_ref *find_matching_val_ref(const std::string &identifier, const type_spec &pattern) const
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
		void push(const type_spec &ts)
		{
			types.push_back(ts);
		}

		const type_spec &top(size_t pos=0)
		{
			if (types.size() < (1+pos)) throw semantic_error_exception();
			return types[types.size()-1-pos];
		}

		type_spec &vtop(size_t pos=0)
		{
			if (types.size() < (1+pos)) throw semantic_error_exception();
			return types[types.size()-1-pos];
		}

		void pop()
		{
			types.pop_back();
		}

	private:
		std::vector<type_spec> types;
	};

	instr make_val_instr(const val_ref &val)
	{
		if (val.mt == mem_type::STACK) {
			return instr {
				.op = op_code::LOAD,
				.res_ts = val.type,
				.val = {.ix=val.ix},
				.arg1_ts = type_spec(),
				.arg2_ts = type_spec()};
		} else if (val.mt == mem_type::LITERAL) {
			if (val.type.first()==basic_type::FUNC) {
				return make_instr(op_code::LOADI, val.type, val.p);
			} else {
				return make_instr(op_code::LOADI, val.type, val.u32);
			}
		} else {
			throw internal_exception();
		}
	}

	bool resolve_rs_id_vals(type_stack &ts, std::vector<instr> &instrs, const scope &s)
	{
		(void)instrs;
		auto &rsts = ts.vtop();
		if (rsts.types.empty()) {
			auto rsinstr = instrs[rsts.iref];
			const val_ref *val = s.find_val_ref(rsinstr.id);
			if (!val) return false;

			rsinstr = make_val_instr(*val);

			rsts = val->type;
		}

		return true;
	}

	bool resolve_call_id_vals(type_stack &ts, std::vector<instr> &instrs, const scope &s)
	{
		if (!resolve_rs_id_vals(ts, instrs, s)) return false;

		auto &lsts = ts.vtop(1);
		if (lsts.types.empty()) {
			// TODO: Make ts: FUNC wildcard RS
			auto &lsinstr = instrs[lsts.iref];

			type_spec type(basic_type::FUNC);
			type.types.push_back(basic_type::UNDECIDED);
			type.types.insert(type.types.end(), ts.top().types.cbegin(), ts.top().types.cend());
			type.types.push_back(basic_type::END);

			const val_ref *val = s.find_matching_val_ref(lsinstr.id, type);
			if (!val) return false;

			lsinstr = make_val_instr(*val);

			//auto &lsts = ts.vtop();
			lsts = val->type;
		}

		return true;
	}

	bool resolve_var_id_vals(type_stack &ts, std::vector<instr> &instrs, const scope &s)
	{
		if (!resolve_rs_id_vals(ts, instrs, s)) return false;

		auto &lsts = ts.vtop(1);
		if (lsts.types.empty()) {
			auto lsinstr = instrs[lsts.iref];

			const val_ref *val = s.find_val_ref(lsinstr.id);
			if (!val) return false;

			lsinstr = make_val_instr(*val);

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
			case tt::RCURLY:
			case tt::RSQUARE:
				ts.vtop().close_tuple();
				break;
			case tt::END:
				{
					type_spec res_ts = ts.top();
					instrs.push_back(instr{
						.op = op_code::END,
						.res_ts = res_ts,
						.val = {},
						.arg1_ts = type_spec(),
						.arg2_ts = type_spec()});
					ts.push(res_ts);
				}
				break;
			case tt::NAME:
				{
					auto instr = make_placeholder_instr(t.text);
					instrs.push_back(instr);
					//auto t = type_spec {
					auto type = type_spec();
					type.iref = instrs.size()-1;
					ts.push(type);
				}
				break;
			case tt::NUM:
				{
					char *endp = nullptr;
#define USE_LOAD_IMMEDIATE
					if (t.str().find('.')!=std::string::npos) { // floating point
						type_spec res_ts = type_spec(basic_type::F);
#ifdef USE_LOAD_IMMEDIATE
						instrs.push_back(instr{
							.op = op_code::LOADI,
							.res_ts = res_ts,
							.val = {.imm_f64 = static_cast<double>(std::strtold(t.get_text().c_str(), &endp))},
							.arg1_ts = type_spec(),
							.arg2_ts = type_spec()});
#else
						auto vix = values.add(std::strtold(t.get_text().c_str(), &endp));
						instrs.push_back(instr{
							.op = op_code::LOAD,
							.res_ts = res_ts,
							.val = {.ix = vix}});
#endif /* USE_LOAD_IMMEDIATE */
						ts.push(res_ts);
					} else { // integral
						type_spec res_ts = type_spec(basic_type::I32);
#ifdef USE_LOAD_IMMEDIATE
						instrs.push_back(instr{
							.op = op_code::LOADI,
							.res_ts = res_ts,
							.val = {.imm_i32 = static_cast<int32_t>(std::strtol(t.get_text().c_str(), &endp, 10))},
							.arg1_ts = type_spec(),
							.arg2_ts = type_spec()});
#else
						auto vix = values.add(std::strtoll(t.get_text().c_str(), &endp, 10));
						instrs.push_back(instr{
							.op = op_code::LOAD,
							.res_ts = res_ts,
							.val = {.ix = vix}});
#endif /* USE_LOAD_IMMEDIATE */
						ts.push(res_ts);
					}
				}
				break;
			case tt::OP_PLUS:
				{
					/*const auto res_type = convert_binary(
						t.get_type(),
						second_last().res_dt,
						last().res_dt);*/

					if (!resolve_var_id_vals(ts, instrs, s)) throw semantic_error_exception();

					type_spec res_ts = ts.top();
					instrs.push_back(instr{
						.op = op_code::ADD,
						.res_ts = res_ts,
						.val = {},
						.arg1_ts = ts.top(1),
						.arg2_ts = ts.top()});
					ts.pop();
					ts.pop();
					ts.push(res_ts);
				}
				break;
			case tt::OP_MINUS:
				{
					/*const auto res_type = convert_binary(
						t.get_type(),
						second_last().res_dt,
						last().res_dt);*/

					if (t.get_arity()==tt_arity::LUNARY) {
						if (!resolve_rs_id_vals(ts, instrs, s)) throw semantic_error_exception();

						type_spec res_ts = ts.top();
						instrs.push_back(instr{
							.op = op_code::NEG,
							.res_ts = res_ts,
							.val = {},
							.arg1_ts = ts.top(),
							.arg2_ts = type_spec()});
						ts.pop();
						ts.push(res_ts);
					} else if (t.get_arity()==tt_arity::BINARY) {
						if (!resolve_var_id_vals(ts, instrs, s)) throw semantic_error_exception();

						type_spec res_ts = ts.top();
						instrs.push_back(instr{
							.op = op_code::SUB,
							.res_ts = res_ts,
							.val = {},
							.arg1_ts = ts.top(1),
							.arg2_ts = ts.top()});
						ts.pop();
						ts.pop();
						ts.push(res_ts);
					} else {
						throw internal_exception();
					}
				}
				break;
			case tt::OP_STAR:
				{
					if (!resolve_var_id_vals(ts, instrs, s)) throw semantic_error_exception();

					type_spec res_ts = ts.top();
					instrs.push_back(instr{
						.op = op_code::MUL,
						.res_ts = res_ts,
						.val = {},
						.arg1_ts = ts.top(1),
						.arg2_ts = ts.top()});
					ts.pop();
					ts.pop();
					ts.push(res_ts);
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

					if (!resolve_rs_id_vals(ts, instrs, s)) throw semantic_error_exception();

					type_spec res_ts = ts.top();

					// "Copy" value to stack
					// Bind id instruction on left side to the value
					auto val = val_ref(
						mem_type::STACK,
						s.frame.add(res_ts.size(), 1),
						res_ts);

					if (instrs.size()<2 || instrs[instrs.size()-2].op!=op_code::ID) throw semantic_error_exception();
					s.ids[instrs[instrs.size()-2].id].push_back(val);

					// TODO: Optimization: Do not copy if immutable variable and right side is immutable

					// Create load value to stack instruction
					instrs.push_back(instr{
						.op = op_code::COPY,
						.res_ts = val.type,
						.val = {.ix=val.ix},
						.arg1_ts = type_spec(),
						.arg2_ts = ts.top()});
					ts.pop();
					ts.push(res_ts);
				}
				break;
#if 0
			case tt::OP_EQ:
				{
					// TODO: Check parent scope(s)
					// If identifier starts with '@'. Only check root scope

					if (!resolve_var_id_vals(ts, instrs, s)) throw semantic_error_exception();

					type_spec res_ts = ts.top();

					auto val = s.find_val_ref(this->curr_id);
					if (!val) {throw semantic_error_exception();}
					if (val->ts != res_ts) {throw semantic_error_exception();}
					if (val->mt != mem_type::STACK) {throw semantic_error_exception();}
					// TODO: Check mutability of 'val'

					instrs.push_back(instr{
						.op = op_code::COPY,
						.res_ts = res_ts,
						.val = {.ix=val->ix},
						.arg1_ts = type_spec(),
						.arg2_ts = ts.top()});
					ts.pop();
					ts.push(res_ts);
				}
				break;
#endif
			case tt::OP_COMMA:
				{
					/*const auto res_type = convert_binary(
						t.get_type(),
						second_last().res_dt,
						last().res_dt);*/

					if (!resolve_var_id_vals(ts, instrs, s)) throw semantic_error_exception();

					type_spec res_ts=build_tuple(ts.top(1), ts.top());

					instrs.push_back(instr{
						.op = op_code::TUPLE,
						.res_ts = res_ts,
						.val = {},
						.arg1_ts = type_spec(),
						.arg2_ts = type_spec()});

					ts.pop();
					ts.pop();
					ts.push(res_ts);
				}
				break;
			case tt::CALL:
				{
					if (!resolve_call_id_vals(ts, instrs, s)) throw semantic_error_exception();

					type_spec res_ts;
					type_spec arg_ts;
					if (ts.top(1).first()==basic_type::FUNC) {
						res_ts = ts.top(1).inner_type(1);
						arg_ts = ts.top(1).inner_type(1+res_ts.types.size());
						
						if (arg_ts!=ts.top()) throw semantic_error_exception();

						instrs.push_back(instr{
							.op = op_code::CALL,
							.res_ts = res_ts,
							.val = {},
							.arg1_ts = ts.top(1),
							.arg2_ts = ts.top()});
					} else {
						throw semantic_error_exception();
					}

					ts.pop();
					ts.pop();
					ts.push(res_ts);
				}
				break;
			default:
				throw not_implemented_exception();
			}
		}
	};
}

#endif /* HILL__BLOCK_HPP_INCLUDED */
