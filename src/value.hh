#ifndef HILL__VALUE_HH_INCLUDED
#define HILL__VALUE_HH_INCLUDED

#include "type.hh"

#include <vector>
#include <string>
#include <sstream>
#include <stdint.h>

namespace hill {

	inline static std::string get_double_str(double val)
	{
		std::stringstream ss;
		if (val == (int64_t)val) {
			ss << std::setprecision(1) << std::fixed << val;
		} else {
			ss << val;
		}

		return ss.str();
	}

	template<typename VT> inline static std::string dump_value(const uint8_t *p)
	{
		if constexpr (std::is_same_v<VT, double>) {
			return get_double_str(*(double *)p);
		} else if constexpr (std::is_same_v<VT, float>) {
			return get_double_str((double)*(float *)p);
		} else {
			std::stringstream ss;
			ss << *(VT *)p;
			return ss.str();
		}
	}

	inline std::string value_to_str(std::vector<basic_type> types, const uint8_t *p)
	{
		if (types.empty()) return "";

		std::ostringstream ss;

		switch (types[0]) {
		case basic_type::FUNC:
			ss << (void *)p; // TODO: Find a sensible way to represent function pointers
			break;
		case basic_type::TUPLE:
			{
				ss << "(";
				std::vector<basic_type> itypes;
				size_t ix;
				for (ix = 1; ix<types.size()-1; ix+=itypes.size(), p+=type_size(itypes)) {
					itypes = inner_type(types, ix);
					if (ix>1) ss << ",";
					ss << value_to_str(itypes, p);
				}
				ss << ")";
			}
			break;
		case basic_type::ARRAY:
			{
				ss << "[";
				std::vector<basic_type> itypes = inner_type(types, 1);
				size_t cnt = (size_t)types[1+itypes.size()];
				for (size_t ix=0; ix<cnt; ++ix, p+=type_size(itypes)) {
					if (ix>0) ss << ",";
					ss << value_to_str(itypes, p);
				}
				ss << "]";
			}
			break;
		case basic_type::BLOCK:
			// TODO:
			break;
		case basic_type::I8: ss << dump_value<int8_t>(p); break;
		case basic_type::I16: ss << dump_value<int16_t>(p); break;
		case basic_type::I32: ss << dump_value<int32_t>(p); break;
		case basic_type::I64:
		case basic_type::I: ss << dump_value<int64_t>(p); break;
		case basic_type::U8: ss << dump_value<uint8_t>(p); break;
		case basic_type::U16: ss << dump_value<uint16_t>(p); break;
		case basic_type::U32: ss << dump_value<uint32_t>(p); break;
		case basic_type::U64:
		case basic_type::U: ss << dump_value<uint64_t>(p); break;
		case basic_type::F32: ss << dump_value<float>(p); break;
		case basic_type::F64:
		case basic_type::F: ss << dump_value<double>(p); break;
		default: throw internal_exception();
		}

		return ss.str();
	}

	struct value {
		value() {}
		value(type ts, const uint8_t *data): ts(ts), data(data, data+ts.mem_size()) {}

		type ts;
		std::vector<uint8_t> data;

		std::string to_str()
		{
			return value_to_str(ts.types, data.data());
		}
	};
}

#endif /* HILL__VALUE_HH_INCLUDED */
