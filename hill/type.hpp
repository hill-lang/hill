#ifndef TYPE_HPP_INCLUDED
#define TYPE_HPP_INCLUDED

#include <string>

namespace hill {

	enum class basic_type {
		UNDECIDED,
		I,
		I32,
		F
	};

	struct type_desc {
		type_desc(basic_type basic_type): basic_type(basic_type) {}
		bool operator==(const type_desc &other) {return this->basic_type==other.basic_type;}
		bool operator<(const type_desc &other) const
		{
			return static_cast<int>(this->basic_type)<static_cast<int>(other.basic_type);
		}
		basic_type basic_type;
	};
}

#endif /* TYPE_HPP_INCLUDED */
