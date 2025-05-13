#ifndef TYPE_HPP_INCLUDED
#define TYPE_HPP_INCLUDED

#include <string>
#include <vector>
#include <numeric>

namespace hill {

	enum class basic_type {
		UNDECIDED,
		STR,
		I, I8, I16, I32, I64,
		U, U8, U16, U32, U64,
		F, F32, F64, /*F128,*/
		ARRAY,
		END,
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
		case basic_type::UNDECIDED: return "@undecided";
		case basic_type::STR: return "@str";
		case basic_type::I: return "@i";
		case basic_type::I8: return "@i8";
		case basic_type::I16: return "@i16";
		case basic_type::I32: return "@i32";
		case basic_type::I64: return "@i64";
		case basic_type::U: return "@u";
		case basic_type::U8: return "@u8";
		case basic_type::U16: return "@u16";
		case basic_type::U32: return "@u32";
		case basic_type::U64: return "@u64";
		case basic_type::F: return "@f";
		case basic_type::F32: return "@f32";
		case basic_type::F64: return "@f64";
		//case basic_type::F128: return "@f128";
		case basic_type::ARRAY: return "@array";
		case basic_type::END: return ")";
		default: return "<UNKNOWN>";
		}
	}

	struct type_spec {
		type_spec() {}
		type_spec(basic_type bt): types{bt} {}
		type_spec(const std::vector<basic_type> &bts): types(bts) {}
		type_spec(const type_spec &other): types(other.types) {}
		bool operator==(const type_spec &other) const {return this->types == other.types;}
		bool operator<(const type_spec &other) const {return this->types < other.types;}

		std::vector<basic_type> types;

		std::string to_str() const
		{
			std::ostringstream ss;

			ss << '(';
			for (auto &bt: types) {
				ss << basic_type_str(bt);
			}
			return ss.str();
		}

		size_t size() const
		{
			return std::accumulate(
				types.begin(),
				types.end(),
				(size_t)0,
				[](size_t sum, auto &bt){return sum+basic_type_size(bt);});
		}
	};

	struct data_type {
		data_type(): bt(basic_type::UNDECIDED), mut(false) {}
		data_type(basic_type bt): bt(bt), mut(false) {}
		//data_type(std::vector<data_type> tuple_types): bt(basic_type::TUPLE), tuple_types(tuple_types), mut(false) {}
		data_type(const data_type &other): bt(other.bt), tuple_types(other.tuple_types), mut(other.mut) {}
		bool operator==(const data_type &other) {return this->bt==other.bt;}
		bool operator<(const data_type &other) const
		{
			return this->bt < other.bt;
		}

		basic_type bt;
		std::vector<data_type> tuple_types;
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
