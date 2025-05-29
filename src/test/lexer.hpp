#ifndef HILL__TEST__LEXER_INCLUDED
#define HILL__TEST__LEXER_INCLUDED

#include "../token.hpp"
#include "../lexer.hpp"
#include "../utils/console.hpp"

#include "test/support.hpp"

namespace hill::test {

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

	inline bool lexer(utils::junit_session &test_session)
	{
		auto suite = test_session.add_suite("Test.Lexer");

		bool ok = true;

		std::cout << "Lexer testing:\n";

		for (size_t ix=0; ix<sizeof lexer_tests/sizeof lexer_tests[0]; ++ix) {
			utils::timer timer;

			auto src_ss = get_src(lexer_tests[ix].src);
			auto exp_ss = get_src(lexer_tests[ix].expected);

			if (src_ss.str().empty()) {
				std::cout << "Cannot read source " << utils::color(lexer_tests[ix].src, utils::ccolor::YELLOW) << '\n';
				ok = false;
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

			std::cout << " Test " << test(
				suite, timer.elapsed_sec(),
				lexer_tests[ix].src,
				exp_ss.str().c_str(),
				ss.str().c_str(),
				&ok);
		}

		return ok;
	}
}

#endif /* HILL__TEST__LEXER_INCLUDED */
