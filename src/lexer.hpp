#ifndef LEXER_HPP_INCLUDED
#define LEXER_HPP_INCLUDED

#include "token.hpp"
#include "lang_spec.hpp"
#include "exceptions.hpp"

#include <algorithm>
#include <istream>
#include <string>
#include <string_view>
#include <sstream>

namespace hill {

	struct lexer {
		lexer() = default;

		int lix = 0, cix = 0;
		int plix = -1, pcix = -1;

		int get(std::istream &istr)
		{
			if (istr.eof()) {
				return 0;
			}
			int ch = istr.get();

			plix = lix;
			pcix = cix;

			if (ch=='\n') {
				++lix;
				cix = 0;
			} else {
				++cix;
			}

			return ch;
		}

		int peek(std::istream &istr)
		{
			return istr.peek();
		}

		void unget(std::istream &istr)
		{
			lix = plix;
			plix = -1;
			cix = pcix;
			pcix = -1;

			istr.unget();
		}

		token get_token(std::istream &istr)
		{
			auto &ops = lang_spec::get().get_tt_patterns();

			int slix = lix, scix = cix;

			if (istr.eof()) return token(tt::END, "", slix, scix);

			std::ostringstream text;

			auto ch = peek(istr);

			// Whitespace
			if (std::isspace(ch)) {
				while (std::isspace(peek(istr))) {
					text.put(ch);
					get(istr);
				}

				return token(tt::WHITESPACE, text.str(), slix, scix);
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
				return token(tt::STRING, text.str(), slix, scix);
			}

			// Character
			if (ch=='\'') {
				throw not_implemented_exception();
			}

			// Comments
			if (ch=='/') {
				text.put(ch);
				get(istr);
				ch=peek(istr);

				if (ch=='/') {
					get(istr);
					while (ch && ch!='\n') {
						text.put(ch);
						ch = get(istr);
					}
					return token(tt::COMMENT, text.str(), slix, scix);
				} else if (ch=='*') {
					get(istr);
					int prev=-1;
					while (ch && (text.tellp()<3 || prev!='*' || ch!='/')) {
						text.put(ch);
						prev = ch;
						ch = get(istr);
					}
					text.put(ch);
					return token(tt::COMMENT, text.str(), slix, scix);
				} else {
					ch = text.str()[0];
					text.str("");
					text.clear();
					unget(istr);
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

				return token(tt::NUM, text.str(), slix, scix);
			}

			// Names
			if (std::isalpha(ch) || ch=='_') {
				while (std::isalnum(ch) || ch=='_') {
					text.put(ch);
					get(istr);
					ch = peek(istr);
				}
				//unget(istr);

				return token(tt::NAME, text.str(), slix, scix);
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

				return token(tt, text.str(), slix, scix);
			}

			// Failed to find a token
			return token(tt::END, "", slix, scix);
		}
	};
}

#endif /* LEXER_HPP_INCLUDED */
