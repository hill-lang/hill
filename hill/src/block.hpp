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
		scope scope;

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

		instr last()
		{
			if (instrs.size()<1) throw semantic_error_exception();
			return instrs[instrs.size()-1];
		}

		instr second_last()
		{
			if (instrs.size()<2) throw semantic_error_exception();
			return instrs[instrs.size()-2];
		}

		void add(const token &t)
		{
			switch (t.get_type()) {
			case tt::END:
				instrs.push_back(instr(op_code::END,
					last().res_dt,
					data_type(basic_type::UNDECIDED),
					data_type(basic_type::UNDECIDED)));
				break;
			case tt::NAME:
				instrs.push_back(instr(op_code::ID,
					data_type(basic_type::UNDECIDED),
					data_type(basic_type::UNDECIDED),
					data_type(basic_type::UNDECIDED)));
				break;
			case tt::NUM:
				{
					char *endp;
					if (t.str().find('.')!=std::string::npos) { // floating point
						auto vix = values.add(std::strtold(t.get_text().c_str(), &endp));
						instrs.push_back(instr(op_code::LOAD, data_type(basic_type::F), vix));
					} else { // integral
						auto vix = values.add(std::strtoll(t.get_text().c_str(), &endp, 10));
						instrs.push_back(instr(op_code::LOAD, data_type(basic_type::I), vix));
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
					instrs.push_back(instr(op_code::ADD, res_dt, second_last().res_dt, last().res_dt));
				}
				break;
			case tt::OP_MINUS:
				{
					/*const auto res_type = convert_binary(
						t.get_type(),
						second_last().res_dt,
						last().res_dt);*/

					data_type res_dt = last().res_dt;
					instrs.push_back(instr(op_code::SUB, res_dt, second_last().res_dt, last().res_dt));
				}
				break;
			case tt::OP_COLON_EQ:
				{
					/*const auto res_type = convert_binary(
						t.get_type(),
						second_last().res_dt,
						last().res_dt);*/

					data_type res_dt = last().res_dt;

					// "Copy" value to stack
					// Bind id instruction on left side to the value
					auto val = val_ref(
						mem_type::STACK,
						scope.frame.add(res_dt.size(), 1),
						res_dt);
					scope.ids[t.str()] = val;

					// TODO: Optimization: Do not copy if immutable variable and right side is immutable
				
					// Create load value to stack instruction
					instrs.push_back(instr(op_code::COPY, val.dt, val.ix, last().res_dt));
				}
				break;
			}
		}
	};
}

#endif /* BLOCK_HPP_INCLUDED */
