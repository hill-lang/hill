#ifndef INSTR_HPP_INCLUDED
#define INSTR_HPP_INCLUDED

#include "type.hpp"

#include <string>
#include <stdint.h>

namespace hill {

	enum class instr_kind {
		ID, // Unbound identifier
		VAL, // Value (literal or calculated by analyzer/optimizer)
		OP, // Operation (op-code)
		FRREF // Frame value reference (frame ref and frame index)
	};

	union instr_val {
		long long i;
		long double f;
		int32_t i32;
		uint32_t u32;
	};

	struct instr {
		instr(instr_kind kind, data_type dt, size_t vix=SIZE_MAX): kind(kind), dt(dt), vix(vix) {}
		instr_kind kind;
		data_type dt;
		size_t vix;
	};
}

#endif /* INSTR_HPP_INCLUDED */
