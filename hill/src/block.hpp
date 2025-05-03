#ifndef BLOCK_HPP_INCLUDED
#define BLOCK_HPP_INCLUDED

namespace hill {

	struct val_ref {
		// TODO: Refer to literal_values, og calculated ones
		bool literal_ref;
		size_t ix;
		data_type dt;
	};

	struct scope { // The names and values
		std::map<std::string, val_ref> ids;
		// frame_def frame;
	};

	struct literal_values {
		std::vector<uint8_t> mem;

		template<typename VT> size_t add(VT val)
		{
			mem.resize(mem.size() + mem.size() % alignof(VT) + sizeof val);
			size_t ix = mem.size() - sizeof val;
			*((VT *)(mem.data() + ix)) = val;
			return ix;
		}

		template<typename VT> VT get(size_t ix)
		{
			return *((VT *)mem.data() + ix);
		}
	};

	struct block { // The code
		scope scope;

		literal_values values;
		std::vector<instr> instrs;

		instr last()
		{
			if (instrs.size()<1) throw semantic_error_exception();
			return instrs[instrs.size()-1];
		}

		instr second_last()
		{
			if (instrs.size()<2) throw semantic_error_exception();
			return instrs[instrs.size()-2];
		}

		void add(const token &t)
		{
			switch (t.get_type()) {
			case tt::NAME:
				instrs.push_back(instr(instr_kind::ID, data_type(basic_type::UNDECIDED)));
				break;
			case tt::NUM:
				char *endp;
				if (t.str().find('.')!=std::string::npos) { // floating point
					auto vix = values.add(std::strtold(t.str().c_str(), &endp));
					instrs.push_back(instr(instr_kind::VAL, data_type(basic_type::F), vix));
				} else { // integral
					auto vix = values.add(std::strtoll(t.str().c_str(), &endp, 10));
					instrs.push_back(instr(instr_kind::VAL, data_type(basic_type::I), vix));
				}
				break;
			case tt::OP_COLON_EQ:
				const auto &res_type = convert_binary(
					t.get_type(),
					second_last().dt,
					last().dt);

				// Bind id instruction on left side to the value (possibly converting to new type)
				
				 
				// Create load value to stack instruction
				break;
			}
		}
	};
}

#endif
