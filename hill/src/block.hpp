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

	struct block { // The code
		scope s;

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

		const instr &last() const
		{
			if (instrs.size()<1) throw semantic_error_exception();
			return instrs[instrs.size()-1];
		}

		const instr &second_last() const
		{
			if (instrs.size()<2) throw semantic_error_exception();
			return instrs[instrs.size()-2];
		}

		void add(const token &t)
		{
			switch (t.get_type()) {
			case tt::END:
				instrs.emplace_back(instr{
					.op = op_code::END,
					.res_ts = last().res_ts});
				break;
			case tt::NAME:
				instrs.emplace_back(instr{.op = op_code::ID});
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
							.val = {.imm_f64 = (double)std::strtold(t.get_text().c_str(), &endp)},
							.arg1_ts = type_spec(),
							.arg2_ts = type_spec()});
#else
						auto vix = values.add(std::strtold(t.get_text().c_str(), &endp));
						//instrs.emplace_back(op_code::LOAD, type_spec(basic_type::F), vix);
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
							.val = {.imm_i64 = std::strtoll(t.get_text().c_str(), &endp, 10)},
							.arg1_ts = type_spec(),
							.arg2_ts = type_spec()});
#else
						auto vix = values.add(std::strtoll(t.get_text().c_str(), &endp, 10));
						//instrs.emplace_back(op_code::LOAD, type_spec(basic_type::I), vix);
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

					type_spec res_ts = last().res_ts;
					//instrs.emplace_back(op_code::ADD, res_ts, second_last().res_ts, last().res_ts);
					instrs.push_back(instr{
						.op = op_code::ADD,
						.res_ts = res_ts,
						.arg1_ts = second_last().res_ts,
						.arg2_ts = last().res_ts});
				}
				break;
			case tt::OP_MINUS:
				{
					/*const auto res_type = convert_binary(
						t.get_type(),
						second_last().res_dt,
						last().res_dt);*/

					type_spec res_ts = last().res_ts;
					//instrs.emplace_back(op_code::SUB, res_ts, second_last().res_ts, last().res_ts);
					instrs.push_back(instr{
						.op = op_code::SUB,
						.res_ts = res_ts,
						.arg1_ts = second_last().res_ts,
						.arg2_ts = last().res_ts});
				}
				break;
			case tt::OP_COLON_EQ:
				{
					/*const auto res_type = convert_binary(
						t.get_type(),
						second_last().res_dt,
						last().res_dt);*/

					type_spec res_ts = last().res_ts;

					// "Copy" value to stack
					// Bind id instruction on left side to the value
					auto val = val_ref(
						mem_type::STACK,
						s.frame.add(res_ts.size(), 1),
						res_ts);
					s.ids[t.str()] = val;

					// TODO: Optimization: Do not copy if immutable variable and right side is immutable

					// Create load value to stack instruction
					//instrs.emplace_back(op_code::COPY, val.ts, val.ix, last().res_ts);
					instrs.push_back(instr{
						.op = op_code::COPY,
						.res_ts = val.ts,
						.val = {.ix=val.ix},
						.arg2_ts = last().res_ts});
				}
				break;
			case tt::OP_COMMA:
				{
					/*const auto res_type = convert_binary(
						t.get_type(),
						second_last().res_dt,
						last().res_dt);*/

					type_spec res_ts = type_spec(second_last().res_ts, last().res_ts);

					auto val = val_ref(
						mem_type::STACK,
						s.frame.add(res_ts.size(), 1),
						res_ts);
					s.ids[t.str()] = val;

					//instrs.emplace_back(op_code::TUPLE, val.ts, val.ix);
					instrs.push_back(instr{
						.op = op_code::TUPLE,
						.res_ts = val.ts,
						.val = {.ix = val.ix}});
				}
				break;
			default:
				break;
			}
		}
	};
}

#endif /* BLOCK_HPP_INCLUDED */
