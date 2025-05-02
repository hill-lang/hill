#ifndef BLOCK_HPP_INCLUDED
#define BLOCK_HPP_INCLUDED

namespace hill {

	struct frame_def { // Actual data storage
		std::vector<std::uint64_t> data;
		size_t alloc(size_t n)
		{
			auto ix = data.size();
			for (size_t i=0; i<n; ++i) data.push_back(0);
			return ix;
		}
		void push(std::uint64_t v)
		{
			data.push_back(v);
		}
	};

	struct val_ref { // Data ref
		frame_def *frame;
		size_t frame_ix;
		const data_type *type;
	};

	struct scope { // The names and values
		std::map<std::string, val_ref> ids;
		frame_def frame;
	};

	struct literal_values {
		std::vector<uint8_t> mem;

		template<typename VT> size_t add(VT val)
		{
			mem.resize(mem.size() + mem.size() % alignof(VT) + sizeof val);
			size_t ix = mem.size() - sizeof val;
			*((VT *)mem.data() + ix) = val;

			return ix;
		}

		template<typename VT> VT get(size_t ix)
		{
			return *((VT *)mem.data() + ix);
		}
	};

	struct block { // The code
		scope scope;

		std::vector<instr> instrs;

		void add(const token &t)
		{
			switch (t.get_type()) {
			case tt::NAME:
				// Create unbound id instruction
				break;
			case tt::NUM:
				// Create value instruction
				break;
			case tt::OP_COLON_EQ:
				// Allocate space for right side value on the stack frame
				// Bind id instruction on left side to the value
				// Create load value to stack instruction
				break;
			}

			auto instr = make_instr(t);

			switch (t.get_arity()) {
			case tt_arity::NULLARY:
				break;
			case tt_arity::LUNARY: throw not_implemented_exception();
			case tt_arity::RUNARY: throw not_implemented_exception();
			case tt_arity::BINARY:
				if (instrs.size()<2) throw semantic_error_exception();
				size_t left_ix = instrs.size()-2;
				size_t right_ix = instrs.size()-1;
				const auto &res_type = convert_binary(
					t.get_type(),
					instrs[left_ix].dt,
					instrs[right_ix].dt);

				if (instrs[left_ix].kind==instr_kind::ID) {
					if (t.get_type()==tt::OP_COLON_EQ) {
						if (scope.ids.contains(t.get_text())) throw semantic_error_exception();

						val_ref v = {
							.frame = &scope.frame,
							.frame_ix = scope.frame.alloc(1), // TODO: Handle larger objects
							.type = &res_type,
						};
						scope.ids[t.get_text()] = v;
					}
				} else throw semantic_error_exception();
				
				instr.dt = res_type;

				break;
			}

			instrs.push_back(instr);
		}
	};
}

#endif
