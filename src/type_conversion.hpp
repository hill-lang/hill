#ifndef HILL__TYPE_CONVERSION_HPP_DEFINED
#define HILL__TYPE_CONVERSION_HPP_DEFINED

#include "type.hpp"

#include <map>

namespace hill {

	inline const std::map<const type, std::map<const type, const type>> &get_conversions()
	{
		static std::map<const type, std::map<const type, const type>> conversions = {
			{type(basic_type::UNDECIDED),
				{
					{type(basic_type::I), type(basic_type::I32)},
					{type(basic_type::I32), type(basic_type::I32)}
				},
			}
		};

		return conversions;
	}

	inline const type convert(const type &target, const type &src)
	{
		if (target==type(basic_type::UNDECIDED)) {
			if (src==type(basic_type::I) || src==type(basic_type::I32)) return type(basic_type::I32);
			else return target;
		} else return target;
	}

	inline const type convert_binary(tt op, const type &left, const type &right)
	{
		switch (op) {
		case tt::OP_COLON_EQ:
			return convert(left, right);
		default: throw not_implemented_exception();
		}
	}
}

#endif /* HILL__TYPE_CONVERSION_HPP_DEFINED */
