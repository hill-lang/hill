#ifndef EVALUATOR_HPP_INCLUDED
#define EVALUATOR_HPP_INCLUDED

#include <vector>
#include <stdint.h>
#include <stddef.h>
#include <algorithm>
#include <string.h>
#include <sstream>

namespace hill {
	template <typename VT> std::string dump_value(const uint8_t *p)
	{
		std::stringstream ss;

		ss << *(VT *)p;

		return ss.str();
	}

	inline std::string dump_value(const type_spec &ts, const uint8_t *p)
	{
		std::stringstream ss;

		if (ts.types.size()>1) {
			ss << '(';
		}

		int ix = 0;
		for (auto t: ts.types) {
			if (ix++>0) {
				ss << ',';
			}
			switch (t) {
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
			//case basic_type::START: ss << dump_value(nts, p); break;
			default: break; /* Throw? Look for custom implemetation? */
			}

			p += basic_type_size(t);
		}

		if (ts.types.size()>1) {
			ss << ')';
		}

		return ss.str();
	}

	struct stack {
		std::vector<uint8_t> mem;

		uint8_t *data()
		{
			return mem.data();
		}

		uint8_t *push_alloc(size_t size)
		{
			size_t start_ix = mem.size();
			mem.resize(start_ix + size);
			return mem.data() + start_ix;
		}

		void push(size_t size, const uint8_t *data)
		{
			uint8_t *p = push_alloc(size);
			memcpy(p, data, size);
		}

		const uint8_t *top(size_t size)
		{
			return mem.data() + mem.size() - size;
		}

		void pop(size_t size)
		{
			mem.resize(mem.size() - size);
		}

		template<typename T> T pop()
		{
			T ret = *(T *)top(sizeof(T));
			pop(sizeof(T));
			return ret;
		}
	};

	struct evaluator {
		stack s;

		void evaluate(const block &b)
		{
			s.push_alloc(b.s.frame.size()); // Allocate stack frame

			for (auto &ins : b.instrs) {
				switch (ins.op) {
				case op_code::END: end(ins); break;
				case op_code::ID: break;
				case op_code::LOAD: load(ins, b.values); break;
				case op_code::COPY: copy(ins); break;
				case op_code::ADD: add(ins); break;
				case op_code::SUB: sub(ins); break;
				case op_code::TUPLE: tuple(ins); break;
				}
			}
		}

	private:
		void end(const instr &ins)
		{
			std::cout << "END"
				<< " ts:" << ins.res_ts.to_str()
				<< " val:" << dump_value(ins.res_ts, s.top(ins.res_ts.size()))
				<< '\n';
		}

		void load(const instr &ins, const literal_values &values)
		{
			uint8_t *p = s.push_alloc(ins.res_ts.size());
			values.copy(ins.ix, p, ins.res_ts.size());
		}

		void copy(const instr &ins)
		{
			// TODO: Type conversion?
			const uint8_t *src = s.top(ins.arg2_ts.size());
			uint8_t *dst = s.data() + ins.ix;
			memcpy(dst, src, ins.arg2_ts.size());
			s.pop(ins.arg2_ts.size());
			s.push(ins.arg2_ts.size(), dst);
		}

		template<typename T> void add()
		{
			T right = s.pop<T>();
			T left = s.pop<T>();
			T res = left + right;
			s.push(sizeof res, (uint8_t *)&res);
		}
		void add(const instr &ins)
		{
			// TODO: Type conversion?
			// Maybe type conversion is its own instruction and handled by the analizer?

			switch (ins.res_ts.types[0]) {
			case basic_type::I8: add<int8_t>(); break;
			case basic_type::I16: add<int16_t>(); break;
			case basic_type::I32: add<int32_t>(); break;
			case basic_type::I64:
			case basic_type::I: add<int64_t>(); break;
			case basic_type::U8: add<uint8_t>(); break;
			case basic_type::U16: add<uint16_t>(); break;
			case basic_type::U32: add<uint32_t>(); break;
			case basic_type::U64:
			case basic_type::U: add<uint64_t>(); break;
			case basic_type::F32: add<float>(); break;
			case basic_type::F64:
			case basic_type::F: add<double>(); break;
			default: break; /* Throw? Look for custom implemetation? */
			}
		}

		template<typename T> void sub()
		{
			T right = s.pop<T>();
			T left = s.pop<T>();
			T res = left - right;
			s.push(sizeof res, (uint8_t *)&res);
		}
		void sub(const instr &ins)
		{
			// TODO: Type conversion?
			// Maybe type conversion is its own instruction and handled by the analizer?

			switch (ins.res_ts.types[0]) {
			case basic_type::I8: sub<int8_t>(); break;
			case basic_type::I16: sub<int16_t>(); break;
			case basic_type::I32: sub<int32_t>(); break;
			case basic_type::I64:
			case basic_type::I: sub<int64_t>(); break;
			case basic_type::U8: sub<uint8_t>(); break;
			case basic_type::U16: sub<uint16_t>(); break;
			case basic_type::U32: sub<uint32_t>(); break;
			case basic_type::U64:
			case basic_type::U: sub<uint64_t>(); break;
			case basic_type::F32: sub<float>(); break;
			case basic_type::F64:
			case basic_type::F: sub<double>(); break;
			default: break; /* Throw? Look for custom implemetation? */
			}
		}

		void tuple(const instr &ins)
		{
/*			const uint8_t *right = s.top(ins.arg2_ts.size());
			s.pop(ins.arg2_ts.size());
			const uint8_t *left = s.top(ins.arg1_ts.size());
			s.pop(ins.arg1_ts.size());

			uint8_t *p = s.push_alloc(ins.res_ts.size());*/

			// Make tuple
			// The top of the stack already looks like a tuple
		}
	};
}

#endif /* EVALUATOR_HPP_INCLUDED */
