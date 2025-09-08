#ifndef HILL__ANALYZER_HH_INCLUDED
#define HILL__ANALYZER_HH_INCLUDED

#include "lexer.hh"
#include "type.hh"
#include "type_conversion.hh"
#include "instr.hh"

#include <memory>
#include <cstdint>

namespace hill {

	enum class hl_op_code {
		VAL,
		ADD,
	};

	struct hl_instr {
		hl_op_code op;
		type res_type=type();
		union {
			//size_t ix;

			//uint8_t u8;
			//uint16_t u16;
			//uint32_t u32;
			//uint64_t u64;

			//int8_t i8;
			//int16_t i16;
			int32_t i32;
			//int64_t i64;

			//float f32;
			double f64;

			//std::string s;

			//void *p;
		} val={0};
		type arg1_type=type(), arg2_type=type();
	};

	struct type_stack {
		void push(const type &ts)
		{
			types.push_back(ts);
		}

		const type &ctop(size_t pos=0)
		{
			if (types.size() < (1+pos)) throw internal_exception();
			return types[types.size()-1-pos];
		}

		type &top(size_t pos=0)
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

	struct scope {
		std::shared_ptr<scope> parent;
	};

	struct block { // The code
		scope s;
		type_stack ts;
		//block *inner_block = nullptr;

		//literal_values values;
		std::vector<hl_instr> instrs;

		bool add(const token &t)
		{
			switch (t.get_type()) {
#if 0
			case tt::LPAR:
				break;
			case tt::LCURLY:
				// TODO: Start analyzing inner block
				inner_block = new block();
				break;
			case tt::LSQUARE:
				break;
			case tt::RPAR:
				ts.vtop().close_tuple();
				break;
			case tt::RCURLY:
				return false; // End of inner block
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
				return false; // End of main block
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
#endif

			case tt::NUM:
				{
					char *endp = nullptr;

					if (t.str().find('.')!=std::string::npos) { // floating point (just double for now)
						type res_type = type(basic_type::F64);
						instrs.push_back(hl_instr {
							.op = hl_op_code::VAL,
							.res_type = res_type,
							.val = {.f64 = static_cast<double>(std::strtold(t.get_text().c_str(), &endp))}});
						res_type.iref = instrs.size()-1;
						ts.push(res_type);
					} else { // integral
						type res_type = type(basic_type::I32);
						instrs.push_back(hl_instr {
							.op = hl_op_code::VAL,
							.res_type = res_type,
							.val = {.i32 = static_cast<int32_t>(std::strtol(t.get_text().c_str(), &endp, 10))}});
						res_type.iref = instrs.size()-1;
						ts.push(res_type);
					}
				}
				break;

#
			case tt::OP_PLUS:
				{
/*					if (!resolve_var_id_vals(ts, instrs, s))
						throw semantic_error_exception(error_code::UNDEFINED_ID);*/

					type res_type = convert_type_binary(hl_op_code::ADD, ts.top(1), ts.top());
					instrs.push_back(hl_instr {
						.op = hl_op_code::ADD,
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
#if 0
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
			case tt::OP_DOT: // Member access
				{
					if (!resolve_ls_id_vals(ts, instrs, s)) 
						throw semantic_error_exception(error_code::UNDEFINED_ID);

					auto arg1_type = ts.top(1);
					auto arg2_type = ts.top();

					if (arg1_type.first()!=basic_type::TUPLE)
						throw semantic_error_exception(error_code::MEMBER_ACCESS_ON_NO_TUPLE);

					auto &rsinstr = instrs[arg2_type.iref];
					auto id = rsinstr.id;

					auto res_type = get_tuple_elm_type(arg1_type, id);

					size_t ix = arg1_type.mem_size() - get_tuple_elm_mem_offset(arg1_type, id);

					instrs.push_back(instr{
						.op = op_code::TUPLE_ELM,
						.res_type = res_type,
						.val = {.ix = ix},
						.arg1_type = ts.top(1),
						.arg2_type = ts.top()});

					ts.pop();
					ts.pop();
					res_type.iref = instrs.size()-1;
					ts.push(res_type);
				}
				break;
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
#endif
			default:
				throw not_implemented_exception();
			}

			return true;
		}
	};

	struct analyzer {
		analyzer() = default;

		block main;

		void set_trunk(const std::shared_ptr<scope> &s)
		{
			main.s.parent = s;
		}

		void analyze_token(const token &t)
		{
			//std::cout<<"analyzer: "<< t.to_str() <<'\n';

			main.add(t);
		}

		void analyze(const std::vector<token> &rpn)
		{
			/*for (auto &t: rpn) {
				std::cout << " - " << t.to_str();
			}
			std::cout << std::endl;*/
			for (auto &t: rpn) {
				analyze_token(t);
			}

			//std::cout << main.to_str();
		}

		const block &get_main_block() const
		{
			return main;
		}
	};
}

#endif /* HILL__ANALYZER_HH_INCLUDED */
