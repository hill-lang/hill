#ifndef TYPE_HPP_INCLUDED
#define TYPE_HPP_INCLUDED

#include <string>

namespace hill {

	enum class basic_type {
		UNDECIDED,
		STR,
		I, I8, I16, I32, I64,
		U, U8, U16, U32, U64,
		F, F32, F64, /*F128,*/
	};

	constexpr size_t basic_type_size(basic_type bt)
	{
		switch (bt) {
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
		//case basic_type::F128: return 16u;
		default: return 0u;
		}
	}

	constexpr const char *basic_type_str(basic_type bt)
	{
		switch (bt) {
		case basic_type::UNDECIDED: return "UNDECIDED";
		case basic_type::STR: return "STR";
		case basic_type::I: return "I";
		case basic_type::I8: return "I8";
		case basic_type::I16: return "I16";
		case basic_type::I32: return "I32";
		case basic_type::I64: return "I64";
		case basic_type::U: return "U";
		case basic_type::U8: return "U8";
		case basic_type::U16: return "U16";
		case basic_type::U32: return "U32";
		case basic_type::U64: return "U64";
		case basic_type::F: return "F";
		case basic_type::F32: return "F32";
		case basic_type::F64: return "F64";
		//case basic_type::F128: return "F128";
		default: return "<UNKNOWN>";
		}
	}

	struct data_type {
		data_type(): bt(basic_type::UNDECIDED), mut(false) {}
		data_type(basic_type bt): bt(bt), mut(false) {}
		bool operator==(const data_type &other) {return this->bt ==other.bt;}
		bool operator<(const data_type &other) const
		{
			return this->bt < other.bt;
		}

		basic_type bt;
		bool mut;

		std::string to_str() const
		{
			return basic_type_str(this->bt);
		}

		size_t size() const
		{
			return basic_type_size(this->bt);
		}
	};
}

#endif /* TYPE_HPP_INCLUDED */
