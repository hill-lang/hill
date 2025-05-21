#ifndef BLOCK_HPP_INCLUDED
#define BLOCK_HPP_INCLUDED

#include "val_ref.hpp"

#include <string>
#include <vector>
#include <map>

namespace hill {

	struct scope { // The names and values
		std::map<std::string, val_ref> ids;
		frame_def frame;
	};

	struct type_stack {
		std::vector<type_spec> types;

		void push(const type_spec &ts)
		{
			types.push_back(ts);
		}

		const type_spec &top(size_t pos=0)
		{
			if (types.size() < (1+pos)) throw semantic_error_exception();
			return types[types.size()-1-pos];
		}

		void pop(size_t size)
		{
			types.resize(types.size() - size);
		}
	};

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
			case tt::END:
				instrs.push_back(instr{
					.op = op_code::END,
					.res_ts = ts.top()});
				break;
			case tt::NAME:
				instrs.push_back(instr{.op = op_code::ID});
				break;
			case tt::NUM:
				{
					char *endp = nullptr;
#define USE_LOAD_IMMEDIATE
					if (t.str().find('.')!=std::string::npos) { // floating point
#ifdef USE_LOAD_IMMEDIATE
						instrs.push_back(instr{
							.op = op_code::LOADI,
							.res_ts = type_spec(basic_type::F),
							.val = {.imm_f64 = (double)std::strtold(t.get_text().c_str(), &endp)}});
#else
						auto vix = values.add(std::strtold(t.get_text().c_str(), &endp));
						instrs.push_back(instr{
							.op = op_code::LOAD,
							.res_ts = type_spec(basic_type::F),
							.val = {.ix = vix}});
#endif /* USE_LOAD_IMMEDIATE */
					} else { // integral
#ifdef USE_LOAD_IMMEDIATE
						instrs.push_back(instr{
							.op = op_code::LOADI,
							.res_ts = type_spec(basic_type::I),
							.val = {.imm_i64 = std::strtoll(t.get_text().c_str(), &endp, 10)}});
#else
						auto vix = values.add(std::strtoll(t.get_text().c_str(), &endp, 10));
						instrs.push_back(instr{
							.op = op_code::LOAD,
							.res_ts = type_spec(basic_type::I),
							.val = {.ix = vix}});
#endif /* USE_LOAD_IMMEDIATE */
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
				}
				break;
			case tt::OP_MINUS:
				{
					/*const auto res_type = convert_binary(
						t.get_type(),
						second_last().res_dt,
						last().res_dt);*/

					type_spec res_ts = ts.top();
					instrs.push_back(instr{
						.op = op_code::SUB,
						.res_ts = res_ts,
						.arg1_ts = ts.top(1),
						.arg2_ts = ts.top()});
				}
				break;
			case tt::OP_COLON_EQ:
				{
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
					s.ids[t.str()] = val;

					// TODO: Optimization: Do not copy if immutable variable and right side is immutable

					// Create load value to stack instruction
					instrs.push_back(instr{
						.op = op_code::COPY,
						.res_ts = val.ts,
						.val = {.ix=val.ix},
						.arg2_ts = ts.top()});
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
				}
				break;
			default:
				break;
			}
		}
	};
}

#endif /* BLOCK_HPP_INCLUDED */
