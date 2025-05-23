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
			{":tests/lexer-mess.hill", ":tests/lexer-mess.exp"},
		};

		inline bool lexer()
		{
			bool ok = true;

			int ix;

			std::cout << "Lexer testing:\n";

			for (ix = 0; ix<sizeof lexer_tests/sizeof lexer_tests[0]; ++ix) {
				auto src_ss = get_src(lexer_tests[ix].src);
				auto exp_ss = get_src(lexer_tests[ix].expected);

				if (src_ss.str().empty()) {
					std::cerr << "Cannot read source " << utils::setcolor(utils::console_color::YELLOW) << lexer_tests[ix].src << utils::resetcolor() << '\n';
					continue;
				}

				std::stringstream ss;
				::hill::lexer l;
				::hill::token t;
				int ii=0;
				while ((t = l.get_token(src_ss)).get_type()!=::hill::tt::END) {
					if (ii++>0) ss << ',';
					ss << t.to_str();
				}

				std::cout << " Test  " << utils::escape_string(lexer_tests[ix].src[0]==':' ? (lexer_tests[ix].src+1) : lexer_tests[ix].src);
				if (!strcmp(exp_ss.str().c_str(), ss.str().c_str())) {
					std::cout << utils::setcolor(utils::console_color::GREEN) << " PASSED\n" << utils::resetcolor();
				} else {
					std::cout << ' '
						<< utils::setcolor(utils::console_color::WHITE)
						<< utils::setbgcolor(utils::console_color::RED)
						<< "FAILED"
						<< utils::resetcolor()
						<< ": Expected: " << utils::escape_string(exp_ss.str().c_str())
						<< " - Actual: " << utils::escape_string(ss.str().c_str()) << '\n';
					ok = false;
				}
			}

			return ok;
		}

	}
}

#endif
 /* TEST__LEXER_INCLUDED */
