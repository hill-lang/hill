#ifndef TYPE_CONVERSION_HPP_DEFINED
#define TYPE_CONVERSION_HPP_DEFINED

#include "type.hpp"
#include "lang_spec.hpp"

#include <map>

namespace hill {

	inline const std::map<const type *, std::map<const type *, const type *>> &get_conversions()
	{
		static std::map<const type *, std::map<const type *, const type *>> conversions = {
			{&type_tag<undecided>(),
				{
					{&type_tag<i>(), &type_tag<i>()},
					{&type_tag<i32>(), &type_tag<i32>()}
				},
			}
		};

		return conversions;
	}

	inline const type &convert(const type &target, const type &src)
	{
		if (is_type<undecided>(target)) {
			if (is_type<i>(src) || is_type<i32>(src)) return type_tag<i32>();
			else return target;
		} else return target;
	}

	inline const type &convert_binary(tt op, const type &left, const type &right)
	{
		switch (op) {
		case tt::OP_COLON_EQ:
			return convert(left, right);
		default: throw not_implemented_exception();
		}
	}
}

#endif /* TYPE_CONVERSION_HPP_DEFINED */
