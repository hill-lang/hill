#ifndef PARSER_HPP_INCLUDED
#define PARSER_HPP_INCLUDED

#include "lexer.hpp"
#include "analyzer.hpp"

#include <memory>
#include <istream>
#include <iostream>
#include <stack>

void error(const char *msg)
{
}

namespace hill {

	struct token_queue {
		token pull_token(std::istream &istr)
		{
			token curr = std::move(this->next_token);
			while ((this->next_token = get_token(istr)).ws());
			return curr;
		}

		const token &get_next_token() const {return this->next_token;}

	private:
		token next_token = token(tt::START, "");
	};

	struct parser {
		std::stack<token> op_stack;
		analyzer analyzer;

		void put_token(token t)
		{
			std::cout << t.str() << '\n';

			analyzer.analyze_token(std::move(t));
		}

		void parse_token(token t)
		{
			if (t.val()) {
				put_token(std::move(t));
			} else if (t.lgroup()) {
				op_stack.push(std::move(t));
			} else if (t.rgroup()) {
				// TODO:
				while (!op_stack.empty() && !op_stack.top().lgroup()) {
					put_token(std::move(op_stack.top()));
					op_stack.pop();
				}
				// TODO: Consider checking if grouping tokens matches in token type
				op_stack.pop();
				put_token(std::move(t));
			} else if (t.op()) {
				if (t.lassoc()) {
					while (!op_stack.empty() && !op_stack.top().lgroup() && op_stack.top().prec() <= t.prec()) {
						put_token(std::move(op_stack.top()));
						op_stack.pop();
					}
				} else if (t.rassoc()) {
					while (!op_stack.empty() && !op_stack.top().lgroup() && op_stack.top().prec() < t.prec()) {
						put_token(std::move(op_stack.top()));
						op_stack.pop();
					}
				} else {
					while (!op_stack.empty() && !op_stack.top().lgroup() && op_stack.top().prec() <= t.prec()) {
						put_token(std::move(op_stack.top()));
						op_stack.pop();
					}
				}

				if (t.get_actual_arity()==tt_arity::RUNARY) put_token(std::move(t));
				else op_stack.push(std::move(t));

				// TODO: Handle short circuit
			} else if (t.end()) {
				while (!op_stack.empty()) {
					put_token(std::move(op_stack.top()));
					op_stack.pop();
				}
			}
		}

		void error_token(token t)
		{
			std::cerr<<"ERROR: " << t.str() << '\n';
			std::exit(EXIT_FAILURE); // TODO: Handle errors properly
		}

		void parse(std::istream &istr)
		{
			token_queue queue;

			token prev_t = token(tt::START, "");
			token t;

			while (!(t = queue.pull_token(istr)).end()) {
				if (t.error()) {
					error("Lexing failed");
				}

				const auto &next_t = &queue.get_next_token();

				if (prev_t.vend() && t.vbegin()) {
					parse_token(token(tt::CALL, ""));
				}

				if (t.op()) {
					if (prev_t.vend()) {
						if ((next_t->vbegin() || next_t->has_arity(tt_arity::LUNARY))
								&& t.has_arity(tt_arity::BINARY)) {
							t.set_actual_arity(tt_arity::BINARY);
							parse_token(std::move(t));
						} else {
							if (t.has_arity(tt_arity::RUNARY)) {
								t.set_actual_arity(tt_arity::RUNARY);
								parse_token(std::move(t));
							} else error_token(std::move(t));
						}
					} else {
						if (t.has_arity(tt_arity::LUNARY)) {
							t.set_actual_arity(tt_arity::LUNARY);
							parse_token(std::move(t));
						} else error_token(std::move(t));
					}
				} else {
					t.set_actual_arity(tt_arity::NULLARY);
					parse_token(std::move(t));
				}

				// Use after move??
				prev_t = std::move(t);
			}

			parse_token(token(tt::END, ""));
		}
	};
}

#endif
