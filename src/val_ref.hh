#ifndef HILL__VAL_REF_HH_INCLUDED
#define HILL__VAL_REF_HH_INCLUDED

#include "type.hh"
#include "utils/print.hh"

#include <iomanip>
#include <string>
#include <string.h>
#include <vector>
#include <array>

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
		default: throw internal_exception();
		}
	}

	struct val_ref {
		val_ref(): mt(mem_type::UNDECIDED), ix(SIZE_MAX) {}
		val_ref(mem_type mt, size_t ix, type ts): mt(mt), ix(ix), type(ts) {}
		template<typename VT> val_ref(VT v, basic_type bt): mt(mem_type::LITERAL), type(bt)
		{
			memcpy(this->val, (uint8_t *)&v, sizeof v);
		}
		val_ref(uint32_t v, basic_type bt): mt(mem_type::LITERAL), u32(v), type(bt) {}
		val_ref(void *p, type ts): mt(mem_type::LITERAL), p(p), type(ts) {}

		mem_type mt;
		union {
			size_t ix;
			uint32_t u32;
			void *p;
			uint8_t val[8];
		};
		hill::type type;

		std::string to_str() const
		{
			std::stringstream ss;
			ss << "mt:" << mem_type_str(mt);
			if (mt!=mem_type::UNDECIDED) ss << ':' << ix;
			ss << ":ts:" << type.to_str();
			return ss.str();
		}
	};
}

#endif /* HILL__VAL_REF_HH_INCLUDED */
