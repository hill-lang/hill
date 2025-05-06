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

	struct lexer {
		int lix = 0, cix = 0;
		int last = -1;

		int get(std::istream &istr)
		{
			int ch = istr.get();

			if (ch=='\n') {
				++lix;
				cix = 0;
			} else {
				++cix;
			}

			last = ch;

			return ch;
		}

		int peek(std::istream &istr)
		{
			return istr.peek();
		}

		void unget(std::istream &istr)
		{
			if (last=='\n') {
				--lix;
			} else {
				--cix;
			}
			istr.unget();
		}

		token get_token(std::istream &istr)
		{
			auto &ops = lang_spec::get().get_tt_patterns();

			if (istr.eof()) return token(tt::END, "", lix, cix);

			std::ostringstream text;

			auto ch = get(istr);

			// Whitespace
			if (std::isspace(ch)) {
				while (std::isspace(peek(istr))) {
					text.put(get(istr));
				}
		
				return token(tt::WHITESPACE, text.str(), lix, cix);
			}

			// String
			if (ch=='"') {
				while (peek(istr)!='"') {
					switch (peek(istr)) {
					case '\\': // Skip escaped character
						get(istr);
						break;
					case '\n':
					case '\0': // Unterminated string
						throw not_implemented_exception();
					default: break;
					}
					
					text.put(get(istr));
				}
				get(istr);
				return token(tt::STRING, text.str(), lix, cix);
			}

			// Character
			if (ch=='\'') {
				throw not_implemented_exception();
			}

			// Comments
			if (ch=='/') {
				text.put(ch);
				ch = get(istr);
				if (ch=='/') {
					text.put(ch);
					std::string rest;
					std::getline(istr, rest);
					text<<rest;
					return token(tt::COMMENT, text.str(), lix, cix);
				} else if (ch=='*') {
					text.put(ch);
					std::string rest;
					while (!(rest.length()>0 && rest[rest.length()-1]=='*')) {
						std::getline(istr, rest, '/');
						text<<rest<<'/';
					}
					return token(tt::COMMENT, text.str(), lix, cix);
				} else {
					ch = text.str()[0];
					text.str("");
					text.clear();
					//unget(istr);
				}
			}

			// Numbers
			if (std::isdigit(ch)) {
				while (std::isdigit(ch) || ch=='.' || ch=='\'') {
					text.put(ch);
					ch = get(istr);
				}
				while (std::isalpha(ch)) { // Type specifiers
					text.put(ch);
					ch = get(istr);
				}
				unget(istr);

				return token(tt::NUM, text.str(), lix, cix);
			}

			// Names
			if (std::isalpha(ch) || ch=='_') {
				while (std::isalnum(ch) || ch=='_') {
					text.put(ch);
					ch = get(istr);
				}
				unget(istr);

				return token(tt::NAME, text.str(), lix, cix);
			}

			// Operators etc.
			if (std::ispunct(ch)) {
				std::string match((size_t)1, (char)ch);

				tt tt = tt::END;
			
				auto found = ops.end();
				while ((found=std::find_if(ops.begin(), ops.end(), [&match](auto &it){return it.first.starts_with(match);}))!=ops.end()) {
					text.put(ch);
					tt = found->second;
					ch = get(istr);
					match += (char)ch;
				}
				unget(istr);

				return token(tt, text.str(), lix, cix);
			}

			// Failed to find a token
			return token(tt::END, "", lix, cix);
		}
	};
}

#endif /* LEXER_HPP_INCLUDED */
