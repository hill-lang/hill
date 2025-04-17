#ifndef ANALYZER_HPP_INCLUDED
#define ANALYZER_HPP_INCLUDED

#include "lang_spec.hpp"
#include "lexer.hpp"
#include "type.hpp"
#include "instr.hpp"


#include <cstdlib>
#include <memory>
#include <stack>
#include <iostream>
#include <map>
#include <vector>

namespace hill
{

	class semantic_error_exception: std::exception {
		const char *what() const noexcept override {return "Semantic error";}
	};

	struct block {
		std::vector<instr> instrs;

		void add(const token &t)
		{
			auto instr = make_instr(t);

			

			switch (t.get_actual_arity()) {
			case tt_arity::NULLARY: throw not_implemented_exception();
			case tt_arity::LUNARY: throw not_implemented_exception();
			case tt_arity::RUNARY: throw not_implemented_exception();
			case tt_arity::BINARY:
				if (instrs.size()<2) throw semantic_error_exception();
				size_t left_ix = instrs.size()-2;
				size_t right_ix = instrs.size()-1;
				switch (t.get_type()) {
				case tt::OP_COLON_EQ:
				default:
					throw not_implemented_exception();
				}
				break;
			}

			instrs.push_back(instr);
		}
	};

	struct analyzer {
		analyzer() {
		}

		block main;

		void analyze_token(const token &t)
		{
			std::cout<<"analyzer: "<< t.str() <<'\n';

			main.add(t);

		}
	};

}

#endif