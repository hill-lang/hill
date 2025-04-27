#ifndef TOKEN_HPP_INCLUDED
#define TOKEN_HPP_INCLUDED

#include "lang_spec.hpp"

#include <string>

namespace hill {

	struct token {
		token():
			type(tt::END),
			type_spec(lang_spec::get().get_tt_spec(tt::END)),
			text(""),
			actual_arity(0)
		{}
		token(tt token_type, const std::string &text):
			type(token_type),
			type_spec(lang_spec::get().get_tt_spec(token_type)),
			text(text),
			actual_arity(0)
		{}
		token(token &&other) noexcept:
			type(other.type),
			type_spec(other.type_spec),
			text(std::move(other.text)),
			actual_arity(other.actual_arity)
		{}

		token(token &) = delete;
		token(const token &) = delete;
		token &operator=(token &) = delete;
		token &operator=(const token &) = delete;

		token &operator=(token &&other) noexcept
		{
			if (this != &other) {
				type = other.type;
				type_spec = other.type_spec;
				text = std::move(other.text);
				actual_arity = other.actual_arity;
			}
			return *this;
		}

	private:
		tt type;

		const tt_spec *type_spec;
		std::string text;
		int actual_arity;

	public:
		tt get_type() const {return this->type;}
		const std::string &get_text() const {return this->text;}
		/*std::string_view view_text() const { return this->text; }*/ /* Maybe we want this? */

		bool end() const {return this->type==tt::END || this->type==tt::ERROR;}
		bool error() const {return this->type==tt::ERROR;}

		bool ws() const {return this->type_spec->kind==tt_kind::WS;}
		bool vend() const {return this->type_spec->kind==tt_kind::RGROUP || this->type_spec->kind==tt_kind::VAL;}
		bool vbegin() const {return this->type_spec->kind==tt_kind::LGROUP || this->type_spec->kind==tt_kind::VAL;}
		bool op() const {return this->type_spec->kind==tt_kind::OP;}
		bool val() const {return this->type_spec->kind==tt_kind::VAL;}
		bool lgroup() const {return this->type_spec->kind==tt_kind::LGROUP;}
		bool rgroup() const {return this->type_spec->kind==tt_kind::RGROUP;}
		bool has_arity(int arity) const {return this->type_spec->arity & arity;}
		bool lassoc() const {return this->type_spec->assoc==tt_assoc::LEFT;}
		bool rassoc() const {return this->type_spec->assoc==tt_assoc::RIGHT;}
		bool nassoc() const {return this->type_spec->assoc==tt_assoc::NONE;}
		int prec() const {return this->type_spec->prec;}
		std::string str() const {return this->type_spec->name + " (" + this->text + ")";}

		void set_actual_arity(int a) {this->actual_arity=a;}
		int get_actual_arity() const {return this->actual_arity;}
	};
}

#endif /* TOKEN_HPP_INCLUDED */
