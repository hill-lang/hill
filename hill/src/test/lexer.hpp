#ifndef TEST__LEXER_INCLUDED
#define TEST__LEXER_INCLUDED

#include "token.hpp"
#include "lexer.hpp"


namespace hill {

	namespace test {

		struct {
			const char *src;
			std::vector<struct token> expected_tokens;
		} lexer_tests[]={
			{"1", {
				//token(tt::NUM, "1", 0, 0),
				//token()
			}},
		};

		inline void lexer()
		{
			std::istringstream istr(R"(
// Comment
n = 1 * 10.0f;
/*
Multi line comment
*/
a = b /* Inline comment */ + c;

total := 100'000L;
PI: = 3.1415f;

a b;
/*/
*/
)");

		}

	}
}

#endif
 /* TEST__LEXER_INCLUDED */
