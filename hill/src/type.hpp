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
		type_desc(basic_type basic_type): basic_type(basic_type), mut(false) {}
		bool operator==(const type_desc &other) {return this->basic_type==other.basic_type;}
		bool operator<(const type_desc &other) const
		{
			return this->basic_type < other.basic_type;
		}
		basic_type basic_type;
		bool mut;
	};
}

#endif /* TYPE_HPP_INCLUDED */
