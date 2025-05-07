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

			int s_lix = lix, s_cix = cix;

			if (istr.eof()) return token(tt::END, "", s_lix, s_cix);

			std::ostringstream text;

			auto ch = peek(istr);

			// Whitespace
			if (std::isspace(ch)) {
				while (std::isspace(peek(istr))) {
					text.put(ch);
					get(istr);
				}
		
				return token(tt::WHITESPACE, text.str(), s_lix, s_cix);
			}

			// String
			if (ch=='"') {
				ch = get(istr);
				while (ch!='"') {
					switch (ch) {
					case '\\': // Skip escaped character
						ch = get(istr);
						break;
					case '\n':
					case '\0': // Unterminated string
						throw not_implemented_exception();
					default: break;
					}
					
					text.put(ch);
					get(istr);
				}
				//get(istr);
				return token(tt::STRING, text.str(), s_lix, s_cix);
			}

			// Character
			if (ch=='\'') {
				throw not_implemented_exception();
			}

			// Comments
			if (ch=='/') {
				text.put(ch);
				get(istr);

				if (ch=='/') {
					text.put(ch);
					std::string rest;
					std::getline(istr, rest);
					text<<rest;
					return token(tt::COMMENT, text.str(), s_lix, s_cix);
				} else if (ch=='*') {
					text.put(ch);
					std::string rest;
					while (!(rest.length()>0 && rest[rest.length()-1]=='*')) {
						std::getline(istr, rest, '/');
						text<<rest<<'/';
					}
					return token(tt::COMMENT, text.str(), s_lix, s_cix);
				} else {
					ch = text.str()[0];
					text.str("");
					text.clear();
					unget(istr);
					// unget() cant correctly hande line/column indexes:
					lix = s_lix;
					cix = s_cix;
				}
			}

			// Numbers
			if (std::isdigit(ch)) {
				while (std::isdigit(ch) || ch=='.' || ch=='\'') {
					text.put(ch);
					get(istr);
					ch = peek(istr);
				}
				while (std::isalpha(ch)) { // Type specifiers
					text.put(ch);
					get(istr);
					ch = peek(istr);
				}
				//unget(istr);

				return token(tt::NUM, text.str(), s_lix, s_cix);
			}

			// Names
			if (std::isalpha(ch) || ch=='_') {
				while (std::isalnum(ch) || ch=='_') {
					text.put(ch);
					get(istr);
					ch = peek(istr);
				}
				//unget(istr);

				return token(tt::NAME, text.str(), s_lix, s_cix);
			}

			// Operators etc.
			if (std::ispunct(ch)) {
				std::string match((size_t)1, (char)ch);

				tt tt = tt::END;
			
				auto found = ops.end();
				while ((found=std::find_if(ops.begin(), ops.end(), [&match](auto &it){return it.first.starts_with(match);}))!=ops.end()) {
					text.put(ch);
					get(istr);
					tt = found->second;
					ch = peek(istr);
					match += (char)ch;
				}
				//unget(istr);

				return token(tt, text.str(), s_lix, s_cix);
			}

			// Failed to find a token
			return token(tt::END, "", s_lix, s_cix);
		}
	};
}

#endif /* LEXER_HPP_INCLUDED */
