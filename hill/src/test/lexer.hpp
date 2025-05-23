#ifndef TEST__LEXER_INCLUDED
#define TEST__LEXER_INCLUDED

#include "token.hpp"
#include "lexer.hpp"
#include "utils/console.hpp"

#include "test/support.hpp"


namespace hill {

	namespace test {

		struct {
			const char *src;
			const char *expected;
		} lexer_tests[]={
			{"1", "1:1:NUM(1)"},
			{"a", "1:1:NAME(a)"},
			{"2 + 3", "1:1:NUM(2),1:2:WHITESPACE( ),1:3:OP_PLUS(+),1:4:WHITESPACE( ),1:5:NUM(3)"},
			{"\n1", "1:1:WHITESPACE(\n),2:1:NUM(1)"},
			{":tests/lexer-mess.hill", "1:1:COMMENT(// Comment),2:1:NAME(n),2:2:WHITESPACE( ),2:3:OP_EQ(=),2:4:WHITESPACE( ),2:5:NUM(1),2:6:WHITESPACE( ),2:7:OP_STAR(*),2:8:WHITESPACE( ),2:9:NUM(10.0f),2:14:OP_SEMICOLON(;),2:15:WHITESPACE(\n),3:1:COMMENT(/*\nMulti line comment\n*/),5:3:WHITESPACE(\n),6:1:NAME(a),6:2:WHITESPACE( ),6:3:OP_EQ(=),6:4:WHITESPACE( ),6:5:NAME(b),6:6:WHITESPACE( ),6:7:COMMENT(/* Inline comment */),6:27:WHITESPACE( ),6:28:OP_PLUS(+),6:29:WHITESPACE( ),6:30:NAME(c),6:31:OP_SEMICOLON(;),6:32:WHITESPACE(\n\n),8:1:NAME(total),8:6:WHITESPACE( ),8:7:OP_COLON_EQ(:=),8:9:WHITESPACE( ),8:10:NUM(100'000L),8:18:OP_SEMICOLON(;),8:19:WHITESPACE(\n),9:1:NAME(PI),9:3:OP_COLON_EQ(:),9:4:WHITESPACE( ),9:5:OP_EQ(=),9:6:WHITESPACE( ),9:7:NUM(3.1415f),9:14:OP_SEMICOLON(;),9:15:WHITESPACE(\n\n),11:1:NAME(a),11:2:WHITESPACE( ),11:3:NAME(b),11:4:OP_SEMICOLON(;),11:5:WHITESPACE(\n),12:1:COMMENT(/*/\n*/),13:3:WHITESPACE(\n),14:1:RPAR()),14:2:WHITESPACE(\n)"},
		};

		inline void lexer()
		{
			int ix;

			std::cout << "Lexer testing:\n";

			for (ix = 0; ix<sizeof lexer_tests/sizeof lexer_tests[0]; ++ix) {
				auto istr = get_src(lexer_tests[ix].src);

				if (istr.str().empty()) {
					std::cerr << "Cannot read source " << utils::setcolor(utils::console_color::YELLOW) << lexer_tests[ix].src << utils::resetcolor() << '\n';
					continue;
				}

				std::stringstream ss;
				::hill::lexer l;
				::hill::token t;
				int ii=0;
				while ((t = l.get_token(istr)).get_type()!=::hill::tt::END) {
					if (ii++>0) ss << ',';
					ss << t.to_str();
				}

				std::cout << "Test  " << utils::escape_string(lexer_tests[ix].src[0]==':' ? (lexer_tests[ix].src+1) : lexer_tests[ix].src);
				if (!strcmp(lexer_tests[ix].expected, ss.str().c_str())) {
					std::cout << utils::setcolor(utils::console_color::GREEN) << " PASSED\n" << utils::resetcolor();
				} else {
					std::cout << ' '
						<< utils::setcolor(utils::console_color::WHITE)
						<< utils::setbgcolor(utils::console_color::RED)
						<< "FAILED"
						<< utils::resetcolor()
						<< ": Expected: " << utils::escape_string(lexer_tests[ix].expected)
						<< " - Actual: " << utils::escape_string(ss.str().c_str()) << '\n';
				}
			}
		}

	}
}

#endif
 /* TEST__LEXER_INCLUDED */
