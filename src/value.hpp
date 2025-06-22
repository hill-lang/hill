#ifndef HILL__VALUE_HPP_INCLUDED
#define HILL__VALUE_HPP_INCLUDED

#include "type.hpp"

#include <vector>
#include <string>
#include <sstream>
#include <stdint.h>

namespace hill {

	struct value {
		value(type ts, const uint8_t *data): ts(ts), data(data, data+ts.size()) {}

		type ts;
		std::vector<uint8_t> data;

		std::string to_str()
		{
			std::stringstream ss;

			const uint8_t *p = data.data();

			auto prev_type = basic_type::TUPLE;
			for (auto &type: ts.types) {
				if (prev_type!=basic_type::TUPLE && type!=basic_type::END) ss << ',';

				switch (type) {
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
				case basic_type::TUPLE: ss << '('; break;
				case basic_type::END: ss << ')'; break;
				default: throw internal_exception();
				}

				p += basic_type_size(type);
				prev_type = type;
			}

			return ss.str();
		}

	private:
		static std::string get_double_str(double val)
		{
			std::stringstream ss;
			if (val == (int64_t)val) {
				ss << std::setprecision(1) << std::fixed << val;
			} else {
				ss << val;
			}

			return ss.str();
		}

		template<typename VT> static std::string dump_value(const uint8_t *p)
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
	};
}

#endif /* HILL__VALUE_HPP_INCLUDED */
