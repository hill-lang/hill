#ifndef VAL_REF_HPP_INCLUDED
#define VAL_REF_HPP_INCLUDED

#include "type.hpp"

#include <iomanip>
#include <string>
#include <string.h>
#include <vector>

namespace hill {

	struct literal_values {
		std::vector<uint8_t> mem;

		template<typename VT> size_t add(VT val)
		{
			mem.resize(mem.size() + mem.size() % alignof(VT) + sizeof val);
			size_t start_ix = mem.size() - sizeof val;
			*((VT *)(mem.data() + start_ix)) = val;
			return start_ix;
		}

		size_t add(const char *str)
		{
			size_t size = strlen(str) + 1;
			size_t start_ix = mem.size();
			mem.resize(start_ix + size);
			memcpy(mem.data() + start_ix, str, size);
			return start_ix;
		}

		size_t add(std::string str)
		{
			size_t start_ix = mem.size();
			mem.resize(start_ix + str.length() + 1);
			memcpy(mem.data() + start_ix, str.c_str(), str.length() + 1);
			return start_ix;
		}

		template<typename VT> VT get(size_t ix) const
		{
			return *((VT *)(mem.data() + ix));
		}

		void copy(size_t ix, uint8_t *dst, size_t size) const
		{
			memcpy(dst, mem.data() + ix, size);
		}

		std::string to_str() const
		{
			const size_t col_cnt = 16;

			std::stringstream ss;

			ss << std::left
				<< std::setfill(' ')
				<< std::setw(11)
				<< "Literals";

			ss << std::right
				<< std::hex
				<< std::uppercase
				<< std::setfill('0');
			for (size_t i=0; i<col_cnt; ++i) {
				ss << " " << std::setw(2) << i;
			}
			ss << '\n';

			for (size_t i=0; i<mem.size(); ++i) {
				if (i % col_cnt == 0) {
					ss << "0x"
						<< std::hex
						<< std::uppercase
						<< std::setw(8)
						<< i << ":";
				}
				ss << " "
					<< std::hex
					<< std::uppercase
					<< std::setfill('0')
					<< std::setw(2)
					<< +mem[i];
				if ((i+1) % col_cnt == 0) {
					ss << "  ";
					for (size_t ii=0; ii<col_cnt; ++ii) {
						uint8_t val = mem[i + ii + 1 - col_cnt];
						if (std::isprint(val)) {
							ss << " " << std::nouppercase << val;
						} else {
							ss << " .";
						}
					}
					ss << '\n';
				}
			}

			size_t remailing = col_cnt - (mem.size() % col_cnt);
			if (remailing!=col_cnt) {
				ss << std::setfill(' ') << std::setw(remailing * 3) << "";
				ss << "  ";
				for (size_t ii = 0; ii<remailing; ++ii) {
					uint8_t val = mem[mem.size() + ii - std::min(col_cnt, mem.size())];
					if (std::isprint(val)) {
						ss << " " << std::nouppercase << val;
					} else {
						ss << " .";
					}
				}
				ss << '\n';
			}

			return ss.str();
		}
	};

	struct frame_def {
		size_t ix=0u;

		size_t size() const
		{
			return ix;
		}

		size_t add(size_t size, size_t cnt)
		{
			size_t start_ix = ix;
			ix += ix%size + size*cnt;
			return start_ix;
		}
	};

	enum class mem_type {
		UNDECIDED,
		LITERAL,
		STACK
	};

	struct val_ref {
		val_ref(): mt(mem_type::UNDECIDED), ix(SIZE_MAX), dt(data_type()) {}
		val_ref(mem_type mt, size_t ix, data_type dt, type_spec ts): mt(mt), ix(ix), dt(dt), ts(ts) {}

		mem_type mt;
		size_t ix;
		data_type dt;
		type_spec ts;

		std::string to_str() const
		{
			std::stringstream ss;

			switch (mt) {
			case mem_type::UNDECIDED:
				ss << "mt:UNDECIDED";
				break;
			case mem_type::LITERAL:
				ss << "mt:LITERAL";
				break;
			case mem_type::STACK:
				ss << "mt:STACK";
				break;
			default:;
			}

			ss << ':';
			if (mt!=mem_type::UNDECIDED) ss << ix;
			ss << ":dt:" << dt.to_str();
			ss << ":ts:" << ts.to_str();

			return ss.str();
		}
	};
}

#endif

