#ifndef LEXER_HPP_INCLUDED
#define LEXER_HPP_INCLUDED

#include "token.hpp"
#include "lang_spec.hpp"
#include "exceptions.hpp"

#include <istream>
#include <string>
#include <string_view>
#include <sstream>

namespace hill {

	token get_token(std::istream &istr)
	{
		auto &ops = lang_spec::get().get_tt_patterns();

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

#endif /* LEXER_HPP_INCLUDED */
