#ifndef INSTR_HPP_INCLUDED
#define INSTR_HPP_INCLUDED

#include "type.hpp"

#include <string>
#include <stdint.h>

namespace hill {

	enum class instr_kind {
		ID, // Unbinded identifier
		VAL, // Value (literal or calculated by analyzer/optimizer)
		OP, // Operation (op-code)
		FRREF // Frame value reference (frame ref and frame index)
	};

	union instr_val {
		long long i;
		long double f;
		int32_t i32;
	};

	struct instr {
		instr(instr_kind kind, type_desc type): kind(kind), type(type) {}
		instr_kind kind;
		type_desc type;
		instr_val val;
	};

	instr make_numval_instr(const std::string &s)
	{
		char *endp;
		if (s.find('.')!=std::string::npos) { // floating point
			auto i = instr(instr_kind::VAL, type_desc(basic_type::F));
			i.val.f = std::strtold(s.c_str(), &endp);
			return i;
		} else { // integral
			auto i = instr(instr_kind::VAL, type_desc(basic_type::I));
			i.val.i = std::strtoll(s.c_str(), &endp, 10);
			return i;
		}
	}

	instr make_strval_instr(const std::string &s)
	{
		auto i = instr(instr_kind::VAL, type_desc(basic_type::STR));
		// TODO
		return i;
	}

	instr make_instr(const token &t)
	{
		auto kind = t.get_type_spec()->kind;
		switch (t.get_type()) {
			case tt::NAME:
				return instr(instr_kind::ID, type_desc(basic_type::UNDECIDED));
				break;
			case tt::NUM:
				return make_numval_instr(t.get_text());
				break;
			case tt::STRING:
				return make_strval_instr(t.get_text());
				break;
			default:
				if (kind==tt_kind::OP) {
					return instr(instr_kind::OP, type_desc(basic_type::UNDECIDED));
				} else throw not_implemented_exception();
		}
	}
}

#endif /* INSTR_HPP_INCLUDED */
