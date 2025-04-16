#ifndef ANALYZER_HPP_INCLUDED
#define ANALYZER_HPP_INCLUDED

#include "lang_spec.hpp"
#include "lexer.hpp"

#include <memory>
#include <stack>
#include <iostream>

namespace hill
{

	struct val {
	};

	struct name_val: val {
	};

	struct num_val: val {
	};

	struct op_val: val {
	};

	struct scope {
	};

	struct analyzer {
		std::stack <std::shared_ptr<val>> vals;
		std::stack <std::shared_ptr<scope>> scopes;

		void analyze_token(std::shared_ptr<token> t)
		{
			std::cout<<"analyzer: "<<t->str()<<'\n';

			switch (t->get_token_type()) {
			case tt::NAME:
				vals.push(std::make_shared<name_val>());
				break;
			case tt::NUM:
				vals.push(std::make_shared<num_val>());
				break;
			case tt::OP:
				vals.push(std::make_shared<op_val>());
				break;
			default:;
			}
		}
	};

}

#endif