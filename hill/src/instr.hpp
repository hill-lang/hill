#ifndef INSTR_HPP_INCLUDED
#define INSTR_HPP_INCLUDED

#include "type.hpp"
#include "val_ref.hpp"

#include <string>
#include <stdint.h>

namespace hill {

	enum class op_code {
		ID, // Unbound identifier
		LOAD, // Value (literal or calculated by analyzer/optimizer)
		COPY, // Bind identifier to memory
	};

	struct instr {
		instr(op_code op, data_type res_dt, data_type arg1_dt, data_type arg2_dt):
			op(op), res_dt(res_dt), normal({.arg1_dt=arg1_dt, .arg2_dt=arg2_dt}) {}
		instr(op_code op, size_t ix, data_type dt):
			op(op), res_dt(dt), load({.ix=ix}) {}
		op_code op;

		data_type res_dt;
		union {
			struct {
				data_type arg1_dt;
				data_type arg2_dt;
			} normal;

			struct {
				size_t ix;
			} load;
		};

		std::string to_str() const
		{
			std::stringstream ss;

			switch (op) {
			case op_code::ID:
				ss<<"ID";
				break;
			case op_code::LOAD:
				ss<<"LOAD";
				break;
			case op_code::COPY:
				ss<<"COPY";
				break;
			default:
				ss<<"<UNKNOWN>";
			}

			ss << " res_dt:" << res_dt.to_str();
			if (op==op_code::LOAD) {
				ss << " ix:" << load.ix;
			} else {
				ss << " arg1_dt:" << normal.arg1_dt.to_str();
				ss << " arg2_dt:" << normal.arg2_dt.to_str();
			}

			return ss.str();
		}
	};
}

#endif /* INSTR_HPP_INCLUDED */
