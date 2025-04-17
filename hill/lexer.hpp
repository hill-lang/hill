#ifndef LEXER_HPP_INCLUDED
#define LEXER_HPP_INCLUDED

#include "lang_spec.hpp"

#include <istream>
#include <string>
#include <string_view>
#include <sstream>

namespace hill {

	struct token {
		token():
			type(tt::END),
			type_spec(lang_spec::get()->get_tt_spec(tt::END)),
			text(""),
			actual_arity(0)
		{}
		token(tt token_type, const std::string &text):
			type(token_type),
			type_spec(lang_spec::get()->get_tt_spec(token_type)),
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
				text = std::move(other.text);
				actual_arity = other.actual_arity;
			}
			return *this;
		}

	private:
		tt type;

		const tt_spec &type_spec;
		std::string text;
		int actual_arity;

	public:
		tt get_type() const {return this->type;}
		const std::string &get_text() const {return this->text;}
		/*std::string_view view_text() const { return this->text; }*/ /* Maybe we want this? */

		bool end() const {return this->type==tt::END || this->type==tt::ERROR;}
		bool error() const {return this->type==tt::ERROR;}

		bool ws() const {return this->type_spec.kind==tt_kind::WS;}
		bool vend() const {return this->type_spec.kind==tt_kind::RGROUP || this->type_spec.kind==tt_kind::VAL;}
		bool vbegin() const {return this->type_spec.kind==tt_kind::LGROUP || this->type_spec.kind==tt_kind::VAL;}
		bool op() const {return this->type_spec.kind==tt_kind::OP;}
		bool val() const {return this->type_spec.kind==tt_kind::VAL;}
		bool lgroup() const {return this->type_spec.kind==tt_kind::LGROUP;}
		bool rgroup() const {return this->type_spec.kind==tt_kind::RGROUP;}
		bool has_arity(int arity) const {return this->type_spec.arity & arity;}
		bool lassoc() const {return this->type_spec.assoc==tt_assoc::LEFT;}
		bool rassoc() const {return this->type_spec.assoc==tt_assoc::RIGHT;}
		bool nassoc() const {return this->type_spec.assoc==tt_assoc::NONE;}
		int prec() const {return this->type_spec.prec;}
		std::string str() const {return this->type_spec.name + " (" + this->text + ")";}

		void set_actual_arity(int a) { this->actual_arity=a;}
		int get_actual_arity() const {return this->actual_arity;}
	};

	token get_token(std::istream &istr)
	{
		auto &ops = lang_spec::get()->get_tt_patterns();

		if (istr.eof()) return token(tt::END, "");

		std::ostringstream texts;

		auto ch = istr.get();

		// Whitespace
		if (std::isspace(ch)) {
			while (std::isspace(ch)) {
				texts.put(ch);
				ch = istr.get();
			}
		
			istr.unget();
			return token(tt::WHITESPACE, texts.str());
		}

		// String
		if (ch=='\"') {
			throw not_implemented_exception();
		}

		// Character
		if (ch=='\'') {
			throw not_implemented_exception();
		}

		// Comments
		if (ch=='/') {
			texts.put(ch);
			ch = istr.get();
			if (ch=='/') {
				texts.put(ch);
				std::string rest;
				std::getline(istr, rest);
				texts<<rest;
				return token(tt::COMMENT, texts.str());
			} else if (ch=='*') {
				texts.put(ch);
				std::string rest;
				while (!(rest.length()>0 && rest[rest.length()-1]=='*')) {
					std::getline(istr, rest, '/');
					texts<<rest<<'/';
				}
				return token(tt::COMMENT, texts.str());
			} else {
				ch = texts.str()[0];
				texts.str("");
				texts.clear();
				istr.unget();
			}
		}

		// Numbers
		if (std::isdigit(ch)) {
			while (std::isdigit(ch) || ch=='.' || ch=='_') {
				texts.put(ch);
				ch = istr.get();
			}
			while (std::isalpha(ch)) { // Type specifiers
				texts.put(ch);
				ch = istr.get();
			}
			istr.unget();

			return token(tt::NUM, texts.str());
		}

		// Names
		if (std::isalpha(ch) || ch=='_') {
			while (std::isalnum(ch) || ch=='_') {
				texts.put(ch);
				ch = istr.get();
			}
			istr.unget();

			return token(tt::NAME, texts.str());
		}

		// Operators etc.
		if (std::ispunct(ch)) {
			std::string match((size_t)1, (char)ch);

			tt tt = tt::END;
			
			auto found = ops.end();
			while ((found=std::find_if(ops.begin(), ops.end(), [&match](auto &it){return it.first.starts_with(match);}))!=ops.end()) {
				texts.put(ch);
				tt = found->second;
				ch = istr.get();
				match += (char)ch;
			}
			istr.unget();

			return token(tt, texts.str());
		}

		// Failed to find a token
		return token(tt::END, "");
	}
}

#endif
