#ifndef TYPE_CONVERSION_HPP_DEFINED
#define TYPE_CONVERSION_HPP_DEFINED

#include "type.hpp"
#include "lang_spec.hpp"

#include <map>

namespace hill {

	inline const std::map<const data_type, std::map<const data_type, const data_type>> &get_conversions()
	{
		static std::map<const data_type, std::map<const data_type, const data_type>> conversions = {
			{data_type(basic_type::UNDECIDED),
				{
					{data_type(basic_type::I), data_type(basic_type::I32)},
					{data_type(basic_type::I32), data_type(basic_type::I32)}
				},
			}
		};

		return conversions;
	}

	inline const data_type convert(const data_type &target, const data_type &src)
	{
		if (target==data_type(basic_type::UNDECIDED)) {
			if (src==data_type(basic_type::I) || src==data_type(basic_type::I32)) return data_type(basic_type::I32);
			else return target;
		} else return target;
	}

	inline const data_type convert_binary(tt op, const data_type &left, const data_type &right)
	{
		switch (op) {
		case tt::OP_COLON_EQ:
			return convert(left, right);
		default: throw not_implemented_exception();
		}
	}
}

#endif /* TYPE_CONVERSION_HPP_DEFINED */
