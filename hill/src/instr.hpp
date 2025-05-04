#ifndef INSTR_HPP_INCLUDED
#define INSTR_HPP_INCLUDED

#include "type.hpp"
#include "val_ref.hpp"

#include <string>
#include <stdint.h>

namespace hill {

	enum class op_code {
		ID, // Unbound identifier
		VAL, // Value (literal or calculated by analyzer/optimizer)
		COPY, // Bind identifier to memory
	};

	struct instr {
		instr(op_code op, val_ref res=val_ref(), val_ref arg1=val_ref(), val_ref arg2=val_ref()):
			op(op), res(res), arg1(arg1), arg2(arg2) {}
		op_code op;

		val_ref res;
		val_ref arg1;
		val_ref arg2;

		std::string to_str()
		{
			std::stringstream ss;

			switch (op) {
			case op_code::ID:
				ss<<"ID";
				break;
			case op_code::VAL:
				ss<<"VAL";
				break;
			case op_code::COPY:
				ss<<"COPY";
				break;
			default:
				ss<<"<UNKNOWN>";
			}

			ss << ' '<<res.to_str();
			ss << ' '<<arg1.to_str();
			ss << ' '<<arg2.to_str();

			return ss.str();
		}

		data_type res_dt() const
		{
			return res.dt;
		}
	};
}

#endif /* INSTR_HPP_INCLUDED */
