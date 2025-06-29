#ifndef HILL__TYPE_HH_INCLUDED
#define HILL__TYPE_HH_INCLUDED

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
		BLOCK,
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
		case basic_type::ARRAY:
		case basic_type::BLOCK:
		case basic_type::END:
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
		case basic_type::BLOCK: return "@block(";
		case basic_type::END: return ")";
		default: throw internal_exception();;
		}
	}

	inline std::ostream &operator<<(std::ostream &os, const basic_type &bt)
	{
		os << basic_type_str(bt);
		return os;
	}

	inline bool is_composite_type(basic_type t)
	{
		return t==basic_type::FUNC || t==basic_type::TUPLE || t==basic_type::ARRAY;
	}

	inline std::vector<basic_type> inner_type(std::vector<basic_type> types, size_t ix)
	{
		if (ix>=types.size()) throw internal_exception();
			
		std::vector<basic_type> inner_types;

		if (types[ix]==basic_type::END) {
			return inner_types;
		} else if (is_composite_type(types[ix])) {
			inner_types.push_back(types[ix]);
			int lvl = 1;
			while (lvl>0) {
				if (++ix>=types.size()) throw internal_exception();
				inner_types.push_back(types[ix]);

				if (is_composite_type(types[ix])) ++lvl;
				else if (types[ix]==basic_type::END) --lvl;
			}
		} else {
			inner_types.push_back(types[ix]);
		}

		return inner_types;
	}

	inline std::string type_to_str(std::vector<basic_type> types)
	{
		if (types.empty()) return "";

		std::ostringstream ss;

		switch (types[0]) {
		case basic_type::FUNC:
			{
				ss << "@func(";
				std::vector<basic_type> itypes = inner_type(types, 1);
				ss << type_to_str(itypes);
				ss << ",";
				ss << type_to_str(inner_type(types, itypes.size() + 1));
				ss << ")";
			}
			break;
		case basic_type::TUPLE:
			{
				ss << "(";
				std::vector<basic_type> itypes;
				size_t ix;
				for (ix = 1; ix<types.size()-1; ix+=itypes.size()) {
					itypes = inner_type(types, ix);
					if (ix>1) ss << ",";
					ss << type_to_str(itypes);
				}
				ss << ")";
			}
			break;
		case basic_type::ARRAY:
			{
				ss << "@array(";
				std::vector<basic_type> itypes = inner_type(types, 1);
				ss << type_to_str(itypes);
				ss << ",";
				ss << (size_t)types[itypes.size()+1];
				ss << ")";
			}
			break;
		case basic_type::BLOCK:
			break;
		default:
			ss << types[0];
		}

		return ss.str();
	}

	inline size_t type_size(std::vector<basic_type> types)
	{
		if (types.empty()) return 0;

		std::ostringstream ss;

		switch (types[0]) {
		case basic_type::FUNC:
			return sizeof(void*);
		case basic_type::TUPLE:
			{
				size_t tsize = 0;
				std::vector<basic_type> itypes;
				size_t ix;
				for (ix = 1; ix<types.size()-1; ix+=itypes.size()) {
					itypes = inner_type(types, ix);
					tsize += type_size(itypes);
				}
				return tsize;
			}
		case basic_type::ARRAY:
			{
				std::vector<basic_type> itypes = inner_type(types, 1);
				return type_size(itypes) * (size_t)types[itypes.size()+1];
			}
		case basic_type::BLOCK:
			return 0;
		default:
			return basic_type_size(types[0]);
		}
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
		std::map<std::string, size_t> names_inner_type;
		std::map<std::string, size_t> names_mem_offset;
		size_t num_elms = 0u;
		bool tuple_closed = false;
		size_t iref = SIZE_MAX;
		bool is_pipe_arg = false;

		void close_tuple()
		{
			tuple_closed = true;
		}

		basic_type first() const
		{
			return types.empty() ? basic_type::UNDECIDED : types[0];
		}

		type inner_type(size_t ix) const
		{
			return type(::hill::inner_type(types, ix));
		}

		std::string to_str() const
		{
			return ::hill::type_to_str(types);
		}

		size_t mem_size() const
		{
			return ::hill::type_size(types);
		}
	};

	inline type build_tuple(const type &left, const type &right)
	{
		std::vector<basic_type> types;
		size_t num_elms;

		types.push_back(basic_type::TUPLE);

		if (left.first()==basic_type::TUPLE && !left.tuple_closed) {
			types.insert(types.end(), left.types.cbegin()+1, left.types.cend()-1);
			num_elms = left.num_elms + 1;
		} else {
			types.insert(types.end(), left.types.cbegin(), left.types.cend());
			num_elms = 2;
		}

		types.insert(types.end(), right.types.cbegin(), right.types.cend());

		types.push_back(basic_type::END);

		auto t = type(types);
		std::string elm_name = "_" + std::to_string(num_elms-1);
		t.names_inner_type[elm_name] = left.types.size()-2u+1u;
		t.names_mem_offset[elm_name] = left.mem_size();
		t.num_elms = num_elms;

		return t;
	}

	inline type get_tuple_elm_type(const type &tuple_type, const std::string &id)
	{
		if (!tuple_type.names_inner_type.contains(id)) throw semantic_error_exception(error_code::UNKNOWN_MEMBER_NAME);
		return tuple_type.inner_type(tuple_type.names_inner_type.at(id));
	}

	inline size_t get_tuple_elm_mem_offset(const type &tuple_type, const std::string &id)
	{
		if (!tuple_type.names_mem_offset.contains(id)) throw semantic_error_exception(error_code::UNKNOWN_MEMBER_NAME);
		return tuple_type.names_mem_offset.at(id);
	}
}

#endif /* HILL__TYPE_HH_INCLUDED */
