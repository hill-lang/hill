#ifndef HILL__EVALUATOR_HPP_INCLUDED
#define HILL__EVALUATOR_HPP_INCLUDED

#include "value.hpp"
#include "type.hpp"

#include <vector>
#include <stdint.h>
#include <stddef.h>
#include <algorithm>
#include <string.h>
#include <sstream>

namespace hill {

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

		template<typename T> void push(T val)
		{
			uint8_t *p = push_alloc(sizeof val);
			*(T *)p = val;
		}

		const uint8_t *top(size_t size) const
		{
			return mem.data() + mem.size() - size;
		}

		uint8_t *vtop(size_t size)
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
		evaluator() = default;

		stack s;
		instr result_ins;

		value evaluate(const block &b)
		{
			s.push_alloc(b.s.frame.size()); // Allocate stack frame

			for (auto &ins : b.instrs) {
				switch (ins.op) {
				case op_code::END: end(ins); break;
				case op_code::LOAD: load(ins); break;
				case op_code::LOADL: loadl(ins, b.values); break;
				case op_code::LOADI: loadi(ins); break;
				case op_code::COPY: copy(ins); break;
				case op_code::ADD: add(ins); break;
				case op_code::SUB: sub(ins); break;
				case op_code::MUL: mul(ins); break;
				case op_code::NEG: neg(ins); break;
				case op_code::TUPLE: tuple(ins); break;
				case op_code::CALL: call(ins); break;
				case op_code::PCALL: pcall(ins); break;
				case op_code::ID: break; // throw internal_exception();
				default: throw internal_exception();
				}
			}

			return value(result_ins.res_ts, s.top(result_ins.res_ts.size()));
		}

	private:
		void end(const instr &ins)
		{
			this->result_ins = ins;
		}

		void load(const instr &ins)
		{
			// TODO: What if we want to load from a different stack frame?
			s.push(ins.arg2_ts.size(), s.data() + ins.val.ix);
		}

		void loadl(const instr &ins, const literal_values &values)
		{
			uint8_t *p = s.push_alloc(ins.res_ts.size());
			values.copy(ins.val.ix, p, ins.res_ts.size());
		}

		/**
		 * @brief Load immediate
		 */
		void loadi(const instr &ins)
		{
			uint8_t *p = s.push_alloc(ins.res_ts.size());
			
			switch (ins.res_ts.types[0]) {
			case basic_type::I8: *(int8_t *)p = ins.val.imm_i8; break;
			case basic_type::I16: *(int16_t *)p = ins.val.imm_i16; break;
			case basic_type::I32: *(int32_t *)p = ins.val.imm_i32; break;
			case basic_type::I64:
			case basic_type::I: *(int64_t *)p = ins.val.imm_i64; break;
			case basic_type::U8: *(uint8_t *)p = ins.val.imm_u8; break;
			case basic_type::U16: *(uint16_t *)p = ins.val.imm_u16; break;
			case basic_type::U32: *(uint32_t *)p = ins.val.imm_u32; break;
			case basic_type::U64:
			case basic_type::U: *(uint64_t *)p = ins.val.imm_u64; break;
			case basic_type::F32: *(float *)p = ins.val.imm_f32; break;
			case basic_type::F64:
			case basic_type::F: *(double *)p = ins.val.imm_f64; break;
			case basic_type::FUNC: *(void **)p = ins.val.imm_p; break;
			default: break; /* Throw? Look for custom implemetation? */
			}
		}

		void copy(const instr &ins)
		{
			// TODO: Type conversion?
			const uint8_t *src = s.top(ins.arg2_ts.size());
			uint8_t *dst = s.data() + ins.val.ix;
			memcpy(dst, src, ins.arg2_ts.size());
			s.pop(ins.arg2_ts.size());
			s.push(ins.arg2_ts.size(), dst);
		}

		template<typename T> void add()
		{
			T right = s.pop<T>();
			T left = s.pop<T>();
			s.push<T>(left + right);
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
			s.push<T>(left - right);
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

		template<typename T> void mul()
		{
			T right = s.pop<T>();
			T left = s.pop<T>();
			s.push<T>(left * right);
		}
		void mul(const instr &ins)
		{
			// TODO: Type conversion?
			// Maybe type conversion is its own instruction and handled by the analizer?

			switch (ins.res_ts.types[0]) {
			case basic_type::I8: mul<int8_t>(); break;
			case basic_type::I16: mul<int16_t>(); break;
			case basic_type::I32: mul<int32_t>(); break;
			case basic_type::I64:
			case basic_type::I: mul<int64_t>(); break;
			case basic_type::U8: mul<uint8_t>(); break;
			case basic_type::U16: mul<uint16_t>(); break;
			case basic_type::U32: mul<uint32_t>(); break;
			case basic_type::U64:
			case basic_type::U: mul<uint64_t>(); break;
			case basic_type::F32: mul<float>(); break;
			case basic_type::F64:
			case basic_type::F: mul<double>(); break;
			default: break; /* Throw? Look for custom implemetation? */
			}
		}

		template<typename T> void neg()
		{
			T v = s.pop<T>();
			s.push<T>(-v);
		}
		void neg(const instr &ins)
		{
			switch (ins.res_ts.types[0]) {
			case basic_type::I8: neg<int8_t>(); break;
			case basic_type::I16: neg<int16_t>(); break;
			case basic_type::I32: neg<int32_t>(); break;
			case basic_type::I64:
			case basic_type::I: neg<int64_t>(); break;
			/*case basic_type::U8: neg<uint8_t>(); break;
			case basic_type::U16: neg<uint16_t>(); break;
			case basic_type::U32: neg<uint32_t>(); break;
			case basic_type::U64:
			case basic_type::U: neg<uint64_t>(); break;*/
			case basic_type::F32: neg<float>(); break;
			case basic_type::F64:
			case basic_type::F: neg<double>(); break;
			default: throw internal_exception();
			}
		}

		void tuple(const instr &ins)
		{
			(void)ins; // Unused for now
/*			const uint8_t *right = s.top(ins.arg2_ts.size());
			s.pop(ins.arg2_ts.size());
			const uint8_t *left = s.top(ins.arg1_ts.size());
			s.pop(ins.arg1_ts.size());

			uint8_t *p = s.push_alloc(ins.res_ts.size());*/

			// Make tuple
			// The top of the stack already looks like a tuple
		}

		typedef void (*ifunc)(uint8_t *, const uint8_t *);

		void call(const instr &ins)
		{
			auto func_size = ins.arg1_ts.size();
			auto arg_size = ins.arg2_ts.size();
			auto res_size = ins.res_ts.size();

			uint8_t *p;
			int diff_size = (int)func_size+(int)arg_size-(int)res_size;
			if (diff_size<0) {
				s.push_alloc((size_t)-diff_size);
				p = s.vtop(func_size+arg_size-diff_size);
			} else {
				p = s.vtop(func_size+arg_size);
			}

			//auto func = *(void (**)(uint8_t *, const uint8_t *))p;
			auto func = *(ifunc *)p;

			func(p, p+func_size);

			if (diff_size>0) {
				s.pop((size_t)diff_size);
			}
		}

		void pcall(const instr &ins)
		{
			(void)ins;
			/*auto func_size = ins.arg1_ts.size();
			auto arg_size = ins.arg2_ts.size();
			auto res_size = ins.res_ts.size();

			uint8_t *p;
			int diff_size = (int)func_size+(int)arg_size-(int)res_size;
			if (diff_size<0) {
				s.push_alloc((size_t)-diff_size);
				p = s.vtop(func_size+arg_size-diff_size);
			} else {
				p = s.vtop(func_size+arg_size);
			}

			//auto func = *(void (**)(uint8_t *, const uint8_t *))p;
			auto func = *(ifunc *)p;

			func(p, p+func_size);

			if (diff_size>0) {
				s.pop((size_t)diff_size);
			}*/
		}

	};
}

#endif /* HILL__EVALUATOR_HPP_INCLUDED */
