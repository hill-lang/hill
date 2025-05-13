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
				instrs.emplace_back(op_code::END,
					last().res_dt,
					last().res_ts,
					data_type(basic_type::UNDECIDED),
					type_spec(),
					data_type(basic_type::UNDECIDED),
					type_spec());
				break;
			case tt::NAME:
				instrs.emplace_back(op_code::ID,
					data_type(basic_type::UNDECIDED),
					type_spec(),
					data_type(basic_type::UNDECIDED),
					type_spec(),
					data_type(basic_type::UNDECIDED),
					type_spec());
				break;
			case tt::NUM:
				{
					char *endp;
					if (t.str().find('.')!=std::string::npos) { // floating point
						auto vix = values.add(std::strtold(t.get_text().c_str(), &endp));
						instrs.emplace_back(op_code::LOAD, data_type(basic_type::F), type_spec(basic_type::F), vix);
					} else { // integral
						auto vix = values.add(std::strtoll(t.get_text().c_str(), &endp, 10));
						instrs.emplace_back(op_code::LOAD, data_type(basic_type::I), type_spec(basic_type::I), vix);
					}
				}
				break;
			case tt::OP_PLUS:
				{
					/*const auto res_type = convert_binary(
						t.get_type(),
						second_last().res_dt,
						last().res_dt);*/

					data_type res_dt = last().res_dt;
					type_spec res_ts = last().res_ts;
					instrs.emplace_back(op_code::ADD, res_dt, res_ts, second_last().res_dt, second_last().res_ts, last().res_dt, last().res_ts);
				}
				break;
			case tt::OP_MINUS:
				{
					/*const auto res_type = convert_binary(
						t.get_type(),
						second_last().res_dt,
						last().res_dt);*/

					data_type res_dt = last().res_dt;
					type_spec res_ts = last().res_ts;
					instrs.emplace_back(op_code::SUB, res_dt, res_ts, second_last().res_dt, second_last().res_ts, last().res_dt, last().res_ts);
				}
				break;
			case tt::OP_COLON_EQ:
				{
					/*const auto res_type = convert_binary(
						t.get_type(),
						second_last().res_dt,
						last().res_dt);*/

					data_type res_dt = last().res_dt;
					type_spec res_ts = last().res_ts;

					// "Copy" value to stack
					// Bind id instruction on left side to the value
					auto val = val_ref(
						mem_type::STACK,
						s.frame.add(res_dt.size(), 1),
						res_dt,
						res_ts);
					s.ids[t.str()] = val;

					// TODO: Optimization: Do not copy if immutable variable and right side is immutable

					// Create load value to stack instruction
					instrs.emplace_back(op_code::COPY, val.dt, val.ts, val.ix, last().res_dt, last().res_ts);
				}
				break;
/*			case tt::OP_COMMA:
				{
					// TODO: Build tuple operation

					if (second_last().res_dt.bt==basic_type::TUPLE) {
						auto t = last().res_dt.tuple_types;
						t.push_back(last().res_dt);
					}

					;;
				}
				break;*/
			default:
				break;
			}
		}
	};
}

#endif /* BLOCK_HPP_INCLUDED */
