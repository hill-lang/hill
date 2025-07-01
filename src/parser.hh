#ifndef HILL__PARSER_HH_INCLUDED
#define HILL__PARSER_HH_INCLUDED

#include <memory>
#include <istream>
#include <iostream>
#include <stack>

namespace hill {

	template<typename T> T pop_mv(std::stack<T> &stack)
	{
		T t = std::move(stack.top());
		stack.pop();
		return t;
	}

	struct token_queue {
		template<typename LT> token pull_token(std::istream &istr, LT &lexer)
		{
			token curr = std::move(this->next_token);
			while ((this->next_token = lexer.get_token(istr)).ws());
			return curr;
		}

		const token &peek_token() const {return this->next_token;}

	private:
		token next_token = token(tt::START, "", -1, -1);
	};

	struct parser {
		parser() = default;

		std::stack<token> op_stack;
		std::vector<token> rpn;

		void put_token(token t)
		{
			rpn.emplace_back(std::move(t));
		}

		void parse_token(token t)
		{
			if (t.val()) {
				put_token(std::move(t));
			} else if (t.lgroup()) {
				op_stack.push(std::move(t));
				put_token(std::move(t));
			} else if (t.rgroup()) {
				while (!op_stack.empty() && !op_stack.top().lgroup()) {
					put_token(pop_mv(op_stack));
				}
				// TODO: Consider checking if grouping tokens matches in token type
				op_stack.pop();
				put_token(std::move(t));
			} else if (t.op()) {
				if (t.lassoc()) {
					while (!op_stack.empty() && !op_stack.top().lgroup() && op_stack.top().prec() <= t.prec()) {
						put_token(pop_mv(op_stack));
					}
				} else if (t.rassoc()) {
					while (!op_stack.empty() && !op_stack.top().lgroup() && op_stack.top().prec() < t.prec()) {
						put_token(pop_mv(op_stack));
					}
				} else {
					while (!op_stack.empty() && !op_stack.top().lgroup() && op_stack.top().prec() <= t.prec()) {
						put_token(pop_mv(op_stack));
					}
				}

				if (t.get_arity()==tt_arity::RUNARY) put_token(std::move(t));
				else op_stack.push(std::move(t));

				// TODO: Handle short circuit
			} else if (t.end()) {
				while (!op_stack.empty()) {
					put_token(pop_mv(op_stack));
				}
				put_token(std::move(t));
			}
		}

		void error_token(token t)
		{
			std::cerr << "ERROR: " << t.str() << '\n';
			std::exit(EXIT_FAILURE); // TODO: Handle errors properly
		}

		template<typename LT> void parse(std::istream &istr, LT &lexer)
		{
			token_queue queue;

			token prev_t = token(tt::START, "", -1, -1);
			token t;

			while (!(t = queue.pull_token(istr, lexer)).end()) {
				if (t.error()) {
					throw internal_exception();
				}

				const auto &next_t = &queue.peek_token();

				if (prev_t.vend() && t.vbegin()) {
					auto ct = token(tt::CALL, "", next_t->lix, next_t->cix);
					ct.select_op_spec(tt_arity::BINARY);//op_type_spec = ct.get();
					parse_token(ct);
					//parse_token(token(tt::CALL, "", next_t->lix, next_t->cix));
				}

				if (t.op()) {
					if (prev_t.vend()) {
						prev_t = t.clone();

						if ((next_t->vbegin() || next_t->has_arity(tt_arity::LUNARY))
								&& t.has_arity(tt_arity::BINARY)) {
							t.select_op_spec(tt_arity::BINARY);
							parse_token(std::move(t));

							// TODO: Could be right unary also
						} else {
							if (t.has_arity(tt_arity::RUNARY)) {
								t.select_op_spec(tt_arity::RUNARY);
								parse_token(std::move(t));
							} else error_token(std::move(t));
						}
					} else {
						prev_t = t.clone();

						if (t.has_arity(tt_arity::LUNARY)) {
							t.select_op_spec(tt_arity::LUNARY);
							parse_token(std::move(t));
						} else error_token(std::move(t));
					}
				} else {
					prev_t = t.clone();

					t.select_op_spec(tt_arity::NULLARY);
					parse_token(std::move(t));
				}
			}

			parse_token(token(tt::END, "", -1, -1));
		}

		const std::vector<token> &get_rpn() const
		{
			return rpn;
		}
	};
}

#endif /* HILL__PARSER_HH_INCLUDED */
