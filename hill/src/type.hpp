#ifndef TYPE_HPP_INCLUDED
#define TYPE_HPP_INCLUDED

#include <string>

namespace hill {

	enum class basic_type {
		UNDECIDED,
		STR,
		I, I8, I16, I32, I64,
		U, U8, U16, U32, U64,
		F, F32, F64, F128,
	};

	static constexpr size_t get_type_size(basic_type t)
	{
		switch (t) {
		case basic_type::I: return 8u;
		case basic_type::I8: return 1u;
		case basic_type::I16: return 2u;
		case basic_type::I32: return 4u;
		case basic_type::I64: return 8u;
		case basic_type::U: return 8u;
		case basic_type::U8: return 1u;
		case basic_type::U16: return 2u;
		case basic_type::U32: return 4u;
		case basic_type::U64: return 8u;
		case basic_type::F: return 8u;
		case basic_type::F32: return 4u;
		case basic_type::F64: return 8u;
		case basic_type::F128: return 16u;
		default: return 0u;
		}
	}

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
			return get_type_size(basic_type);
		}
	};
}

#endif /* TYPE_HPP_INCLUDED */
