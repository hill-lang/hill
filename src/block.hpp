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
		std::map<std::string, val_ref> ids;
		frame_def frame;

		std::shared_ptr<scope> parent = nullptr;

		const val_ref *find_val_ref(const std::string &identifier) const
		{
			if (ids.contains(identifier)) {
				return &ids.at(identifier);
			} else if (parent) {
				return parent->find_val_ref(identifier);
			} else {
				return nullptr;
			}
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

		void pop()
		{
			types.pop_back();
		}

	private:
		std::vector<type_spec> types;
	};

	struct block { // The code
		scope s;
		type_stack ts;

		literal_values values;
		std::vector<instr> instrs;

		// Very temp, just to make stuff work
		std::string curr_id;

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
				break;
			case tt::END:
				{
					type_spec res_ts = ts.top();
					instrs.push_back(instr{
						.op = op_code::END,
						.res_ts = res_ts});
					ts.push(res_ts);
				}
				break;
			case tt::NAME:
				{
					this->curr_id = t.text;

					// If this is on the right side of the equal sign
					auto val = s.find_val_ref(t.text);
					if (val) {
						if (val->mt == mem_type::STACK) {
							instrs.push_back(instr{
								.op = op_code::LOAD,
								.res_ts = val->ts,
								.val = {.ix=val->ix}});
						} else if (val->mt == mem_type::LITERAL) {
							if (val->ts.first()==basic_type::FUNC) {
								instrs.push_back(make_instr(op_code::LOADI, val->ts, val->p));
							} else {
								instrs.push_back(make_instr(op_code::LOADI, val->ts, val->u32));
							}
						} else {
							throw internal_exception();
						}
						ts.push(val->ts);
					}
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
							.val = {.imm_f64 = (double)std::strtold(t.get_text().c_str(), &endp)}});
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
							.val = {.imm_i32 = std::strtol(t.get_text().c_str(), &endp, 10)}});
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

					type_spec res_ts = ts.top();
					instrs.push_back(instr{
						.op = op_code::ADD,
						.res_ts = res_ts,
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
						type_spec res_ts = ts.top();
						instrs.push_back(instr{
							.op = op_code::NEG,
							.res_ts = res_ts,
							.arg1_ts = ts.top()});
						ts.pop();
						ts.push(res_ts);
					} else if (t.get_arity()==tt_arity::BINARY) {
						type_spec res_ts = ts.top();
						instrs.push_back(instr{
							.op = op_code::SUB,
							.res_ts = res_ts,
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
					/*const auto res_type = convert_binary(
						t.get_type(),
						second_last().res_dt,
						last().res_dt);*/

					type_spec res_ts = ts.top();
					instrs.push_back(instr{
						.op = op_code::MUL,
						.res_ts = res_ts,
						.arg1_ts = ts.top(1),
						.arg2_ts = ts.top()});
					ts.pop();
					ts.pop();
					ts.push(res_ts);
				}
				break;
			case tt::OP_COLON_EQ:
				{
					// TODO: Check if id already exists in this scope.
					// If it does, that is a compilation error
					// Maybe try to create some nice error message?

					/*const auto res_type = convert_binary(
						t.get_type(),
						second_last().res_dt,
						last().res_dt);*/

					type_spec res_ts = ts.top();

					// "Copy" value to stack
					// Bind id instruction on left side to the value
					auto val = val_ref(
						mem_type::STACK,
						s.frame.add(res_ts.size(), 1),
						res_ts);
					s.ids[this->curr_id] = val;

					// TODO: Optimization: Do not copy if immutable variable and right side is immutable

					// Create load value to stack instruction
					instrs.push_back(instr{
						.op = op_code::COPY,
						.res_ts = val.ts,
						.val = {.ix=val.ix},
						.arg2_ts = ts.top()});
					ts.pop();
					ts.push(res_ts);
				}
				break;
			case tt::OP_EQ:
				{
					// TODO: Check parent scope(s)
					// If identifier starts with '@'. Only check root scope

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
						.arg2_ts = ts.top()});
					ts.pop();
					ts.push(res_ts);
				}
				break;
			case tt::OP_COMMA:
				{
					/*const auto res_type = convert_binary(
						t.get_type(),
						second_last().res_dt,
						last().res_dt);*/

					type_spec res_ts(ts.top(1), ts.top());

					instrs.push_back(instr{
						.op = op_code::TUPLE,
						.res_ts = res_ts});

					ts.pop();
					ts.pop();
					ts.push(res_ts);
				}
				break;
			case tt::CALL:
				{
					type_spec res_ts;
					type_spec arg_ts;
					if (ts.top(1).first()==basic_type::FUNC) {
						res_ts = ts.top().inner_type(1);
						arg_ts = ts.top().inner_type(1+res_ts.types.size());

						instrs.push_back(instr{
							.op = op_code::CALL,
							.res_ts = res_ts,
							.arg1_ts = ts.top(),
							.arg2_ts = arg_ts
							});
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
