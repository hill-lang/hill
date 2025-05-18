#ifndef VAL_REF_HPP_INCLUDED
#define VAL_REF_HPP_INCLUDED

#include "type.hpp"
#include "utils/print.hpp"

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
			return utils::bintostr(mem.data(), mem.size(), 16u, "Literals", true);
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

	constexpr const char *mem_type_str(mem_type mt)
	{
		switch (mt) {
		case mem_type::UNDECIDED: return "UNDECIDED";
		case mem_type::LITERAL: return "LITERAL";
		case mem_type::STACK: return "STACK";
		default: return "<UNKNOWN>";
		}
	}

	struct val_ref {
		val_ref(): mt(mem_type::UNDECIDED), ix(SIZE_MAX) {}
		explicit val_ref(mem_type mt, size_t ix, type_spec ts): mt(mt), ix(ix), ts(ts) {}

		mem_type mt;
		size_t ix;
		type_spec ts;

		std::string to_str() const
		{
			std::stringstream ss;
			ss << "mt:" << mem_type_str(mt);
			if (mt!=mem_type::UNDECIDED) ss << ':' << ix;
			ss << ":ts:" << ts.to_str();
			return ss.str();
		}
	};
}

#endif /* VAL_REF_HPP_INCLUDED */
