#ifndef EVALUATOR_HPP_INCLUDED
#define EVALUATOR_HPP_INCLUDED

#include <vector>
#include <stdint.h>
#include <stddef.h>
#include <algorithm>

namespace hill {

	struct evaluator {
		std::vector<uint8_t> stack;

		void copy_to_stack(const literal_values &values, size_t literal_ix, size_t stack_ix, size_t size)
		{
			stack.resize(std::max(this->stack.size(), stack_ix + size));
			values.copy(literal_ix, this->stack.data() + stack_ix, size);
		}

		void evaluate(const block &b)
		{
			val_ref current_val;
			for (auto &instr: b.instrs) {
				switch (instr.op) {
				case op_code::ID:
					break;
				case op_code::LOAD:
					break;
				case op_code::COPY:
					//copy_to_stack(b.values, instr.arg1.ix, instr.arg2.ix, instr.arg2.dt.size());
					break;
				}
			}
		}
	};
}

#endif /* EVALUATOR_HPP_INCLUDED */
