#ifndef ANALYZER_HPP_INCLUDED
#define ANALYZER_HPP_INCLUDED

#include "lang_spec.hpp"
#include "lexer.hpp"
#include "type.hpp"
#include "type_conversion.hpp"
#include "instr.hpp"
#include "block.hpp"

#include <cstdlib>
#include <memory>
#include <stack>
#include <iostream>
#include <map>
#include <vector>
#include <cstdint>

namespace hill {


	struct analyzer {
		analyzer() {}

		std::vector<instr> instrs;

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

		void analyze(const std::vector<token> &rpn)
		{
			for (auto &t: rpn) {
				analyze_token(t);
			}
		}

		const std::vector<instr> &get_instrs()
		{
			return instrs;
		}
	};

}

#endif /* ANALYZER_HPP_INCLUDED */
