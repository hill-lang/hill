#ifndef INSTR_HPP_INCLUDED
#define INSTR_HPP_INCLUDED

#include "type.hpp"
#include "val_ref.hpp"

#include <string>
#include <stdint.h>

namespace hill {

	enum class op_code {
		END, // End of program
		ID, // Unbound identifier
		LOAD, // Value (literal or calculated by analyzer/optimizer)
		COPY, // Bind identifier to memory,
		ADD, // Arithmetic addition
		SUB, // Arithmetic subtraction
	};

	inline const char *op_code_str(op_code op)
	{
		switch (op) {
		case op_code::END: return "END";
		case op_code::ID: return "ID";
		case op_code::LOAD: return "LOAD";
		case op_code::COPY: return "COPY";
		case op_code::ADD: return "ADD";
		case op_code::SUB: return "SUB";
		default: return "<UNKNOWN>";
		}
	}

	struct instr {
		instr(op_code op, data_type res_dt, type_spec res_ts, data_type arg1_dt, type_spec arg1_ts, data_type arg2_dt, type_spec arg2_ts):
			op(op), res_dt(res_dt), res_ts(res_ts), arg1_dt(arg1_dt), arg1_ts(arg1_ts), arg2_dt(arg2_dt), arg2_ts(arg2_ts) {}
		instr(op_code op, data_type res_dt, type_spec res_ts, size_t ix, data_type arg2_dt, type_spec arg2_ts):
			op(op), res_dt(res_dt), res_ts(res_ts), ix(ix), arg2_dt(arg2_dt), arg2_ts(arg2_ts) {}
		instr(op_code op, data_type dt, type_spec ts, size_t ix):
			op(op), res_dt(dt), res_ts(ts), ix(ix) {}

		op_code op;

		data_type res_dt;
		type_spec res_ts;

		size_t ix;
		data_type arg1_dt;
		type_spec arg1_ts;
		data_type arg2_dt;
		type_spec arg2_ts;

		std::string to_str() const
		{
			std::stringstream ss;

			ss << op_code_str(op);

			ss << " res_dt:" << res_dt.to_str();
			if (op==op_code::LOAD) {
				ss << " ix:" << this->ix;
			} else if (this->op==op_code::COPY) {
				ss << " ix:" << this->ix;
				ss << " arg2_dt:" << this->arg2_dt.to_str();
			} else {
				ss << " arg1_dt:" << this->arg1_dt.to_str();
				ss << " arg2_dt:" << this->arg2_dt.to_str();
			}

			return ss.str();
		}
	};
}

#endif /* INSTR_HPP_INCLUDED */
