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
		instr(op_code op, data_type res_dt, data_type arg1_dt, data_type arg2_dt):
			op(op), res_dt(res_dt), normal({.arg1_dt=arg1_dt, .arg2_dt=arg2_dt}) {}
		instr(op_code op, data_type res_dt, size_t ix, data_type arg2_dt):
			op(op), res_dt(res_dt), copy({.ix=ix, .arg2_dt=arg2_dt}) {}
		instr(op_code op, data_type dt, size_t ix):
			op(op), res_dt(dt), load({.ix=ix}) {}
		instr(const instr &other) {
		}
		~instr() {}

		op_code op;

		data_type res_dt;

		union {
			struct {
				data_type arg1_dt;
				data_type arg2_dt;
			} normal;

			struct {
				size_t ix;
				data_type arg2_dt;
			} copy;

			struct {
				size_t ix;
			} load;
		};

		std::string to_str() const
		{
			std::stringstream ss;

			ss << op_code_str(op);

			ss << " res_dt:" << res_dt.to_str();
			if (op==op_code::LOAD) {
				ss << " ix:" << this->load.ix;
			} else if (this->op==op_code::COPY) {
				ss << " ix:" << this->copy.ix;
				ss << " arg2_dt:" << this->normal.arg2_dt.to_str();
			} else {
				ss << " arg1_dt:" << this->normal.arg1_dt.to_str();
				ss << " arg2_dt:" << this->normal.arg2_dt.to_str();
			}

			return ss.str();
		}
	};
}

#endif /* INSTR_HPP_INCLUDED */
