

#ifndef LEXER_HPP_INCLUDED
#define LEXER_HPP_INCLUDED

#include "lang_spec.hpp"

#include <memory>
#include <string>
#include <map>
#include <algorithm>
#include <istream>
#include <sstream>
#include <cctype>

namespace hill
{
	class not_implemented_exception {
	};

	struct token {
		token():
			type(tt::END),
			token_type_spec(lang_spec::get_tt_spec(tt::END)),
			text(""),
			actual_arity(0)
		{}
		token(tt token_type, const std::string &text):
			type(token_type),
			token_type_spec(lang_spec::get_tt_spec(token_type)),
			text(text),
			actual_arity(0)
		{}

	private:
		tt type;

		const tt_spec *token_type_spec;
		std::string text;
		int actual_arity;

	public:
		tt get_type()
		{
			return type;
		}
		const std::string &get_text()
		{
			return text;
		}
		bool end() const
		{
			return type==tt::END || type==tt::ERROR;
		}
		bool error() const
		{
			return type==tt::ERROR;
		}
		bool ws() const
		{
			return token_type_spec->kind==tt_kind::WS;
		}
		bool vend() const
		{
			return token_type_spec->kind==tt_kind::RGROUP || token_type_spec->kind==tt_kind::VAL;
		}
		bool vbegin() const
		{
			return token_type_spec->kind==tt_kind::LGROUP || token_type_spec->kind==tt_kind::VAL;
		}
		bool op() const
		{
			return token_type_spec->kind==tt_kind::OP;
		}
		bool val() const
		{
			return token_type_spec->kind==tt_kind::VAL;
		}
		bool lgroup() const
		{
			return token_type_spec->kind==tt_kind::LGROUP;
		}
		bool rgroup() const
		{
			return token_type_spec->kind==tt_kind::RGROUP;
		}
		bool has_arity(int arity) const
		{
			return token_type_spec->arity & arity;
		}
		bool lassoc() const
		{
			return token_type_spec->assoc==tt_assoc::LEFT;
		}
		bool rassoc() const
		{
			return token_type_spec->assoc==tt_assoc::RIGHT;
		}
		bool nassoc() const
		{
			return token_type_spec->assoc==tt_assoc::NONE;
		}
		int prec() const
		{
			return token_type_spec->prec;
		}
		/*tt_kind kind() const
		{
			return token_type_spec->kind;
		}*/
		std::string str() const
		{
			return token_type_spec->name + " (" + text + ")";
		}

		void set_actual_arity(int a)
		{
			actual_arity = a;
		}
		int get_actual_arity()
		{
			return actual_arity;
		}
	};

	std::shared_ptr<token> get_token(std::istream &istr)
	{
		auto ops = lang_spec().build_tt_patterns();

		if (istr.eof()) return std::make_shared<token>(tt::END, "");

		std::ostringstream texts;

		auto ch = istr.get();

		// Whitespace
		if (std::isspace(ch)) {
			while (std::isspace(ch)) {
				texts.put(ch);
				ch = istr.get();
			}
		
			istr.unget();
			return std::make_shared<token>(tt::WHITESPACE, texts.str());
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
				return std::make_shared<token>(tt::COMMENT, texts.str());
			} else if (ch=='*') {
				texts.put(ch);
				std::string rest;
				while (!(rest.length()>0 && rest[rest.length()-1]=='*')) {
					std::getline(istr, rest, '/');
					texts<<rest<<'/';
				}
				return std::make_shared<token>(tt::COMMENT, texts.str());
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

			return std::make_shared<token>(tt::NUM, texts.str());
		}

		// Names
		if (std::isalpha(ch) || ch=='_') {
			while (std::isalnum(ch) || ch=='_') {
				texts.put(ch);
				ch = istr.get();
			}
			istr.unget();

			return std::make_shared<token>(tt::NAME, texts.str());
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

			return std::make_shared<token>(tt, texts.str());
		}

		// Failed to find a token
		return std::make_shared<token>(tt::END, "");
	}
}

#endif
