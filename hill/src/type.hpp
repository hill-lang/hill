#ifndef TYPE_HPP_INCLUDED
#define TYPE_HPP_INCLUDED

#include <string>
#include <map>

namespace hill {

	enum class basic_type {
		UNDECIDED,
		STR,
		I,
		I32,
		F
	};

	static std::map<basic_type, size_t> type_sizes = {
		{basic_type::I32, 4u}
	};

	struct data_type {
		data_type(basic_type basic_type): basic_type(basic_type), mut(false) {}
		bool operator==(const data_type &other) {return this->basic_type==other.basic_type;}
		bool operator<(const data_type &other) const
		{
			return this->basic_type < other.basic_type;
		}
		basic_type basic_type;
		bool mut;

		size_t size() const
		{
			return type_sizes[basic_type];
		}
	};
}

#endif /* TYPE_HPP_INCLUDED */
