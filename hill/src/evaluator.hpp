#ifndef EVALUATOR_HPP_INCLUDED
#define EVALUATOR_HPP_INCLUDED

#include <vector>
#include <stdint.h>
#include <stddef.h>
#include <algorithm>
#include <string.h>

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

		const uint8_t *top(size_t size)
		{
			return mem.data() + mem.size() - size;
		}

		void pop(size_t size)
		{
			mem.resize(mem.size() - size);
		}
	};

	struct evaluator {
		stack s;

		void copy_to_stack(const literal_values &values, size_t literal_ix, size_t size)
		{
			uint8_t *p = s.push_alloc(size);
			values.copy(literal_ix, p, size);
		}

		void evaluate(const block &b)
		{
			s.push_alloc(b.scope.frame.ix); // Allocate stack frame

			for (auto &instr: b.instrs) {
				switch (instr.op) {
				case op_code::END:
					std::cout << "END"
						<< " dt:" << instr.res_dt.to_str()
						<< " val:" << *(int *)s.top(instr.res_dt.size())
						<< '\n';
					break;
				case op_code::ID:
					break;
				case op_code::LOAD:
					copy_to_stack(b.values, instr.load.ix, instr.res_dt.size());
					break;
				case op_code::COPY:
					{
						// TODO: Type conversion?
						const uint8_t *src = s.top(instr.copy.arg2_dt.size());
						uint8_t *dst = s.data() + instr.copy.ix;
						memcpy(dst, src, instr.copy.arg2_dt.size());
						s.pop(instr.copy.arg2_dt.size());
						s.push(instr.copy.arg2_dt.size(), dst);
						break;
					}
				case op_code::ADD:
					{
						// TODO: Type conversion?
						// TODO: Data types?
						int64_t left = *(int64_t *)s.top(instr.normal.arg1_dt.size());
						s.pop(instr.normal.arg1_dt.size());
						int64_t right = *(int64_t *)s.top(instr.normal.arg2_dt.size());
						s.pop(instr.normal.arg2_dt.size());
						int64_t res = left + right;
						s.push(sizeof res, (uint8_t *)&res);
						break;
					}
				}
			}
		}
	};
}

#endif /* EVALUATOR_HPP_INCLUDED */
