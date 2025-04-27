#ifndef TYPE_CONVERSION_HPP_DEFINED
#define TYPE_CONVERSION_HPP_DEFINED

#include "type.hpp"
#include "lang_spec.hpp"

#include <map>

namespace hill {

	inline const std::map<const type_desc, std::map<const type_desc, const type_desc>> &get_conversions()
	{
		static std::map<const type_desc, std::map<const type_desc, const type_desc>> conversions = {
			{type_desc(basic_type::UNDECIDED),
				{
					{type_desc(basic_type::I), type_desc(basic_type::I32)},
					{type_desc(basic_type::I32), type_desc(basic_type::I32)}
				},
			}
		};

		return conversions;
	}

	inline const type_desc convert(const type_desc &target, const type_desc &src)
	{
		if (target==type_desc(basic_type::UNDECIDED)) {
			if (src==type_desc(basic_type::I) || src==type_desc(basic_type::I32)) return type_desc(basic_type::I32);
			else return target;
		} else return target;
	}

	inline const type_desc convert_binary(tt op, const type_desc &left, const type_desc &right)
	{
		switch (op) {
		case tt::OP_COLON_EQ:
			return convert(left, right);
		default: throw not_implemented_exception();
		}
	}
}

#endif /* TYPE_CONVERSION_HPP_DEFINED */
