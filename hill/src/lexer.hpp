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

		std::ostringstream text;

		auto ch = istr.get();

		// Whitespace
		if (std::isspace(ch)) {
			while (std::isspace(ch)) {
				text.put(ch);
				ch = istr.get();
			}
		
			istr.unget();
			return token(tt::WHITESPACE, text.str());
		}

		// String
		if (ch=='"') {
			while (istr.peek()!='"') {
				switch (istr.peek()) {
				case '\\': // Skip escaped character
					istr.get();
					break;
				case '\n':
				case '\0': // Unterminated string
					throw not_implemented_exception();
				default: break;
				}
					
				text.put(istr.get());
			}
			istr.get();
			return token(tt::STRING, text.str());
		}

		// Character
		if (ch=='\'') {
			throw not_implemented_exception();
		}

		// Comments
		if (ch=='/') {
			text.put(ch);
			ch = istr.get();
			if (ch=='/') {
				text.put(ch);
				std::string rest;
				std::getline(istr, rest);
				text<<rest;
				return token(tt::COMMENT, text.str());
			} else if (ch=='*') {
				text.put(ch);
				std::string rest;
				while (!(rest.length()>0 && rest[rest.length()-1]=='*')) {
					std::getline(istr, rest, '/');
					text<<rest<<'/';
				}
				return token(tt::COMMENT, text.str());
			} else {
				ch = text.str()[0];
				text.str("");
				text.clear();
				istr.unget();
			}
		}

		// Numbers
		if (std::isdigit(ch)) {
			while (std::isdigit(ch) || ch=='.' || ch=='\'') {
				text.put(ch);
				ch = istr.get();
			}
			while (std::isalpha(ch)) { // Type specifiers
				text.put(ch);
				ch = istr.get();
			}
			istr.unget();

			return token(tt::NUM, text.str());
		}

		// Names
		if (std::isalpha(ch) || ch=='_') {
			while (std::isalnum(ch) || ch=='_') {
				text.put(ch);
				ch = istr.get();
			}
			istr.unget();

			return token(tt::NAME, text.str());
		}

		// Operators etc.
		if (std::ispunct(ch)) {
			std::string match((size_t)1, (char)ch);

			tt tt = tt::END;
			
			auto found = ops.end();
			while ((found=std::find_if(ops.begin(), ops.end(), [&match](auto &it){return it.first.starts_with(match);}))!=ops.end()) {
				text.put(ch);
				tt = found->second;
				ch = istr.get();
				match += (char)ch;
			}
			istr.unget();

			// TODO: This will become an issue with tokens that have multiple specs
			// Maybe we don't assign a 'token type spec' at this time?
			// See token constructor
			return token(tt, text.str());
		}

		// Failed to find a token
		return token(tt::END, "");
	}
}

#endif /* LEXER_HPP_INCLUDED */
