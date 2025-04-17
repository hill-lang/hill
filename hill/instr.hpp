#ifndef INSTR_HPP_INCLUDED
#define INSTR_HPP_INCLUDED

#include "type.hpp"

#include <string>

namespace hill {

	struct instr {
		virtual const type &get_type() const {return type_o<undecided>();}
	};

	struct id: instr {
		id(const std::string &name): name(name) {}
		const type &get_type() const {return type_o<undecided>();}
		std::string name;
	};

	struct ival: instr {
		ival(long long v): v(v) {}
		const type &get_type() const {return type_o<i>();}
		long long v;
	};

	struct fval: instr {
		fval(long double v): v(v) {}
		const type &get_type() const {return type_o<f>();}
		long double v;
	};

	struct op: instr {
		op(tt op_type): op_type(op_type) {}
		const type &get_type() const {return type_o<undecided>();}
		tt op_type;
	};

	instr make_numval(const std::string &s)
	{
		char *endp;
		if (s.find('.')!=std::string::npos) { // floating point
			return ival(std::strtoll(s.c_str(), &endp, 10));
		} else { // integral
			return fval(std::strtold(s.c_str(), &endp));
		}
	}

	instr make_instr(const token &t)
	{
		auto kind = lang_spec::get()->get_tt_spec(t.get_type()).kind;
		switch (t.get_type()) {
			case tt::NAME:
				return id(t.get_text());
				break;
			case tt::NUM:
				return make_numval(t.get_text());
				break;
			default:
				if (kind==tt_kind::OP) {
					return op(t.get_type());
				} else throw not_implemented_exception();
		}
	}
}

#endif
