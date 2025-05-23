#ifndef TYPE_CONVERSION_HPP_DEFINED
#define TYPE_CONVERSION_HPP_DEFINED

#include "type.hpp"

#include <map>

namespace hill {

	inline const std::map<const type_spec, std::map<const type_spec, const type_spec>> &get_conversions()
	{
		static std::map<const type_spec, std::map<const type_spec, const type_spec>> conversions = {
			{type_spec(basic_type::UNDECIDED),
				{
					{type_spec(basic_type::I), type_spec(basic_type::I32)},
					{type_spec(basic_type::I32), type_spec(basic_type::I32)}
				},
			}
		};

		return conversions;
	}

	inline const type_spec convert(const type_spec &target, const type_spec &src)
	{
		if (target==type_spec(basic_type::UNDECIDED)) {
			if (src==type_spec(basic_type::I) || src==type_spec(basic_type::I32)) return type_spec(basic_type::I32);
			else return target;
		} else return target;
	}

	inline const type_spec convert_binary(tt op, const type_spec &left, const type_spec &right)
	{
		switch (op) {
		case tt::OP_COLON_EQ:
			return convert(left, right);
		default: throw not_implemented_exception();
		}
	}
}

#endif /* TYPE_CONVERSION_HPP_DEFINED */
