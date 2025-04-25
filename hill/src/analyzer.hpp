#ifndef ANALYZER_HPP_INCLUDED
#define ANALYZER_HPP_INCLUDED

#include "lang_spec.hpp"
#include "lexer.hpp"
#include "type.hpp"
#include "type_conversion.hpp"
#include "instr.hpp"

#include <cstdlib>
#include <memory>
#include <stack>
#include <iostream>
#include <map>
#include <vector>
#include <cstdint>

namespace hill {

	struct frame { // Actual data storage
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

	struct val { // Data ref
		frame *frame;
		size_t frame_ix;
		const type_desc *type;
	};

	struct scope { // The names and values
		std::map<std::string, val> ids;
		frame frame;
	};

	struct block { // The code
		scope scope;
		std::vector<instr> instrs;

		void add(const token &t)
		{
			auto instr = make_instr(t);

			switch (t.get_actual_arity()) {
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
					instrs[left_ix].type,
					instrs[right_ix].type);

				if (instrs[left_ix].kind==instr_kind::ID) {
					if (t.get_type()==tt::OP_COLON_EQ) {
						if (scope.ids.contains(t.get_text())) throw semantic_error_exception();

						val v = {
							.frame = &scope.frame,
							.frame_ix = scope.frame.alloc(1), // TODO: Handle larger objects
							.type = &res_type,
						};
						scope.ids[t.get_text()] = v;
					}
				} else throw semantic_error_exception();
				
				instr.type = res_type;

				break;
			}

			instrs.push_back(instr);
		}
	};

	struct analyzer {
		analyzer() {}

		block main;

		void analyze_token(const token &t)
		{
			std::cout<<"analyzer: "<< t.str() <<'\n';

			main.add(t);

		}

		void run(block b)
		{
			// TODO: Move this into separate module
			for (const auto &instr: b.instrs) {
				switch (instr.kind) {
				case instr_kind::VAL:
					main.scope.frame.push(instr.val.i32);
					break;
				case instr_kind::ID:
					break;
				case instr_kind::OP:
					break;
				default: throw not_implemented_exception();
				}
			}
		}

		void run()
		{
			run(main);
		}
	};

}

#endif /* ANALYZER_HPP_INCLUDED */
