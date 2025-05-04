#ifndef VAL_REF_HPP_INCLUDED
#define VAL_REF_HPP_INCLUDED

#include "type.hpp"

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

		template<typename VT> VT get(size_t ix)
		{
			return *((VT *)(mem.data() + ix));
		}
	};

	struct frame_def {
		size_t ix=0u;

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
		val_ref(mem_type mt, size_t ix, data_type dt): mt(mt), ix(ix), dt(dt) {}

		mem_type mt;
		size_t ix;
		data_type dt;

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

			return ss.str();
		}
	};
}

#endif

