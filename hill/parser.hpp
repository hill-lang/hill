#ifndef PARSER_HPP_INCLUDED
#define PARSER_HPP_INCLUDED

#include "lexer.hpp"
#include "analyzer.hpp"

#include <istream>
#include <iostream>
#include <stack>

void error(const char *msg)
{
}

namespace hill
{
	struct token_queue {
		std::shared_ptr<token> pull_token(std::istream& istr)
		{
			current_token.swap(next_token);
			while ((next_token = get_token(istr))->ws()) ;
			return current_token;
		}

		std::shared_ptr<token> get_next_token()
		{
			return next_token;
		}

	private:
		std::shared_ptr<token> current_token = std::make_shared<token>(token(tt::START, ""));
		std::shared_ptr<token> next_token = std::make_shared<token>(token(tt::START, ""));
	};


	struct parser {
		std::stack<std::shared_ptr<token>> op_stack;
		analyzer analyzer;

		void put_token(std::shared_ptr<token> t)
		{
			std::cout<<t->str()<<'\n';

			analyzer.analyze_token(t);
		}

		void parse_token(std::shared_ptr<token> t)
		{
			if (t->val()) {
				put_token(t);
			} else if (t->lgroup()) {
				op_stack.push(t);
			} else if (t->rgroup()) {
				// TODO:
				while (!op_stack.empty() && !op_stack.top()->lgroup()) {
					put_token(op_stack.top());
					op_stack.pop();
				}
				// TODO: Consider checking if grouping tokens matches in token type
				op_stack.pop();
				put_token(t);
			} else if (t->op()) {
				if (t->lassoc()) {
					while (!op_stack.empty() && !op_stack.top()->lgroup() && op_stack.top()->prec() <= t->prec()) {
						put_token(op_stack.top());
						op_stack.pop();
					}
				} else if (t->rassoc()) {
					while (!op_stack.empty() && !op_stack.top()->lgroup() && op_stack.top()->prec() < t->prec()) {
						put_token(op_stack.top());
						op_stack.pop();
					}
				} else {
					while (!op_stack.empty() && !op_stack.top()->lgroup() && op_stack.top()->prec() <= t->prec()) {
						put_token(op_stack.top());
						op_stack.pop();
					}
				}

				if (t->get_actual_arity()==tt_arity::RUNARY) put_token(t);
				else op_stack.push(t);

				// TODO: Handle short circuit
			} else if (t->end()) {
				while (!op_stack.empty()) {
					put_token(op_stack.top());
					op_stack.pop();
				}
			}
		}

		void error_token(std::shared_ptr<token> t)
		{
			std::cerr<<"ERROR: "<<t->str()<<'\n';
			std::exit(EXIT_FAILURE); // TODO: Handle errors properly
		}

		void parse(std::istream &istr)
		{
			token_queue queue;

			std::shared_ptr<token> prev_t = std::make_shared<token>(token(tt::START, ""));
			std::shared_ptr<token> t(nullptr);
			std::shared_ptr<token> next_t(nullptr);
			while (!(t=queue.pull_token(istr))->end()) {
				next_t = queue.get_next_token();

				if (prev_t->vend() && t->vbegin()) {
					parse_token(std::make_shared<token>(token(tt::CALL, "")));
				}

				if (t->op()) {
					if (prev_t->vend()) {
						if ((next_t->vbegin() || next_t->has_arity(tt_arity::LUNARY))
								&& t->has_arity(tt_arity::BINARY)) {
							t->set_actual_arity(tt_arity::BINARY);
							parse_token(t);
						} else {
							if (t->has_arity(tt_arity::RUNARY)) {
								t->set_actual_arity(tt_arity::RUNARY);
								parse_token(t);
							} else error_token(t);
						}
					} else {
						if (t->has_arity(tt_arity::LUNARY)) {
							t->set_actual_arity(tt_arity::LUNARY);
							parse_token(t);
						} else error_token(t);
					}
				} else {
					t->set_actual_arity(tt_arity::NULLARY);
					parse_token(t);
				}

				if (t->error()) {
					error("Lexing failed");
				}

				prev_t.swap(t);
			}

			parse_token(std::make_shared<token>(token(tt::END, "")));
		}
	};
}

#endif
