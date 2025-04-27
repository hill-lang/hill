#ifndef INSTR_HPP_INCLUDED
#define INSTR_HPP_INCLUDED

#include "type.hpp"

#include <string>
#include <stdint.h>

namespace hill {

	enum class instr_kind {
		ID,
		VAL,
		OP
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

	instr make_numval(const std::string &s)
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

	instr make_instr(const token &t)
	{
		auto kind = lang_spec::get().get_tt_spec(t.get_type())->kind;
		switch (t.get_type()) {
			case tt::NAME:
				return instr(instr_kind::ID, type_desc(basic_type::UNDECIDED));
				break;
			case tt::NUM:
				return make_numval(t.get_text());
				break;
			default:
				if (kind==tt_kind::OP) {
					return instr(instr_kind::OP, type_desc(basic_type::UNDECIDED));
				} else throw not_implemented_exception();
		}
	}
}

#endif /* INSTR_HPP_INCLUDED */
