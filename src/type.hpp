#ifndef HILL__TYPE_HPP_INCLUDED
#define HILL__TYPE_HPP_INCLUDED

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
		FUNC, // Regular function
		ARRAY,
		TUPLE,
		END,
	};

	constexpr size_t basic_type_size(basic_type bt)
	{
		switch (bt) {
		case basic_type::I8:
		case basic_type::U8:
			return 1u;
		case basic_type::I16:
		case basic_type::U16:
			return 2u;
		case basic_type::I32:
		case basic_type::U32:
		case basic_type::F32:
			return 4u;
		case basic_type::I:
		case basic_type::I64:
		case basic_type::U:
		case basic_type::U64:
		case basic_type::F:
		case basic_type::F64:
			return 8u;
		/*case basic_type::F128:
			return 16u;*/
		case basic_type::FUNC:
			return sizeof(void *);
		case basic_type::TUPLE:
		case basic_type::END:
		case basic_type::ARRAY:
			return 0u;
		default: throw internal_exception();
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
		case basic_type::FUNC: return "@fn";
		//case basic_type::F128: return "@f128";
		case basic_type::TUPLE: return "(";
		case basic_type::ARRAY: return "@array(";
		case basic_type::END: return ")";
		default: throw internal_exception();;
		}
	}

	inline std::ostream &operator<<(std::ostream &os, const basic_type &bt)
	{
		os << basic_type_str(bt);
		return os;
	}

	enum class type_kind {
		PLACEHOLDER,
		DEPENDENT,
		USERDEFINED,
	};

	struct type_dict_entry {
		type_kind kind;
		std::vector<basic_type> types; // For dependent type
		//op_code operation; // For dependent type
	};

	struct type_dict {
		std::vector<type_dict_entry> entries;
	};

	struct type {
		type() = default;
		explicit type(basic_type bt): types{bt} {}
		explicit type(const std::vector<basic_type> &bts): types(bts) {}
		//type_spec(const type_spec &other): types(other.types) {}
		bool operator==(const type &other) const {return this->types == other.types;}
		bool operator!=(const type &other) const {return !(*this==other);}
		bool operator<(const type &other) const {return this->types < other.types;}
		bool matches(const type &other) const {
			for (size_t ix=0; ix<this->types.size(); ++ix) {
				// TODO: First condition is a hack to avoid mismatching missing END in incomplete tuple
				if (this->types[ix]!=basic_type::END && this->types[ix]!=other.types[ix] && this->types[ix]!=basic_type::UNDECIDED) {
					return false;
				}
			}
			return true;
		}
		
		std::vector<basic_type> types;
		bool tuple_closed = false;
		size_t iref = SIZE_MAX;
		bool is_pipe_arg = false;

		basic_type first() const
		{
			return types.empty() ? basic_type::UNDECIDED : types[0];
		}

		bool is_composite_type(basic_type t) const
		{
			return t==basic_type::FUNC || t==basic_type::TUPLE;
		}

		type inner_type(size_t ix) const
		{
			if (ix>=types.size()) throw semantic_error_exception();
			
			std::vector<basic_type> inner_types;

			if (is_composite_type(types[ix])) {
				inner_types.push_back(types[ix]);
				int lvl = 1;
				while (lvl>0) {
					if (++ix>=types.size()) throw semantic_error_exception();
					inner_types.push_back(types[ix]);

					if (is_composite_type(types[ix])) ++lvl;
					else if (types[ix]==basic_type::END) --lvl;
				}
			} else {
				inner_types.push_back(types[ix]);
			}

			return type(inner_types);
		}

		type inner_type_complete(size_t ix) const
		{
			auto type = inner_type(ix);

			if (type.types[0]==basic_type::TUPLE && type.types[type.types.size()-1]!=basic_type::END) {
				type.types.push_back(basic_type::END);
			}

			return type;
		}

		std::string to_str() const
		{
			std::ostringstream ss;

			if (!types.empty() && types[0]==basic_type::ARRAY) {
				ss << "@array(" << types[1] << "," << (int)types[2] << ")";
			} else {
				auto prev_type = basic_type::TUPLE;
				for (auto &type: types) {
					if (prev_type!=basic_type::TUPLE && prev_type!=basic_type::ARRAY && type!=basic_type::END) ss << ',';
					ss << type;

					prev_type = type;
				}
			}

			return ss.str();
		}

		size_t size() const
		{
			if (types.size()>0 && types[0]==basic_type::FUNC) {
				return basic_type_size(basic_type::FUNC); // A function is always the size of a function pointer
			}
			if (types.size()>0 && types[0]==basic_type::ARRAY) {
				return basic_type_size(types[1]) * (size_t)types[2];
			}

			return std::accumulate(
				types.begin(),
				types.end(),
				(size_t)0,
				[](size_t sum, auto &bt){return sum+basic_type_size(bt);});
		}

		void close_tuple()
		{
			tuple_closed = true;
		}
	};

	inline type build_tuple(const type &left, const type &right)
	{
		std::vector<basic_type> types;

		types.push_back(basic_type::TUPLE);

		if (left.types[0]==basic_type::TUPLE && !left.tuple_closed) {
			types.insert(types.end(), left.types.cbegin()+1, left.types.cend()-1);
		} else {
			types.insert(types.end(), left.types.cbegin(), left.types.cend());
		}

		types.insert(types.end(), right.types.cbegin(), right.types.cend());

		types.push_back(basic_type::END);

		return type(types);
	}
}

#endif /* HILL__TYPE_HPP_INCLUDED */
