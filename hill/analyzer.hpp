#ifndef ANALYZER_HPP_INCLUDED
#define ANALYZER_HPP_INCLUDED

#include "lang_spec.hpp"
#include "lexer.hpp"

#include <memory>
#include <stack>
#include <iostream>
#include <map>

namespace hill
{
	struct type {
	};

	struct i: type { // Integer literal of unspecified size
	};

	struct u: type { //Unsigned integer literal of unspecified size
	};

	struct i32: type {
	};

	struct instr {
	};

	struct id: instr {
		id(const std::string &name): name(name) {}
		std::string name;
	};

	struct num: instr {
	};

	struct op: instr {
	};

	struct type: instr {
	};

/*	struct scope {
		//std::map<std::string, val> names;
	};*/

	struct block {
		std::vector<instr> instrs;

		void add(const token &t)
		{
			switch (t.get_type()) {
			case tt::NAME:
				instrs.push_back(id(t.get_text()));
				break;
			case tt::NUM:
				instrs.push_back(num());
				break;
			case tt::OP:
				instrs.push_back(op());
				break;
			default:;
			}
		}
	};

	struct analyzer {
		analyzer() {
			/*scope root;*/

			/*root.names = {
				{"i8", type_val()},
			};*/

			/*scopes.push(std::make_shared<scope>(root));*/
		}

		block main;
		//std::stack <std::shared_ptr<val>> vals;
		//std::stack <std::shared_ptr<scope>> scopes;

		void analyze_token(const token &t)
		{
			std::cout<<"analyzer: "<< t.str() <<'\n';

			main.add(t);

		}
	};

}

#endif