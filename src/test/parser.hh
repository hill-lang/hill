#ifndef HILL__TEST__PARSER_HH_INCLUDED
#define HILL__TEST__PARSER_HH_INCLUDED

#include "../lexer.hh"

#include "./support.hh"

namespace hill::test {

	struct {
		const char *src;
		const char *expected;
	} parser_tests[]={
		{"1", "NUM(1),END()"},
		{"a", "NAME(a),END()"},
		{"2 + 3", "NUM(2),NUM(3),OP_PLUS(+):Binary,END()"},
		{"1 + 2 * 3", "NUM(1),NUM(2),NUM(3),OP_STAR(*):Binary,OP_PLUS(+):Binary,END()"},
		{"(1 + 2) * 3", "LPAR(),NUM(1),NUM(2),OP_PLUS(+):Binary,RPAR()),NUM(3),OP_STAR(*):Binary,END()"},
		{"1 2", "NUM(1),NUM(2),CALL():Binary,END()"},
	};

	inline bool parser(utils::junit_session &test_session)
	{
		auto suite = test_session.add_suite("Test.Parser");

		bool ok = true;

		std::cout << "Parser testing:\n";

		for (size_t ix=0; ix<sizeof parser_tests/sizeof parser_tests[0]; ++ix) {
			utils::timer timer;

			auto src_ss = get_src(parser_tests[ix].src);
			auto exp_ss = get_src(parser_tests[ix].expected);

			if (src_ss.str().empty()) {
				std::cout << "Cannot read source " << utils::color(parser_tests[ix].src, utils::ccolor::YELLOW) << '\n';
				ok = false;
				continue;
			}

			::hill::lexer l;
			::hill::parser p;
			p.parse(src_ss, l);

			std::stringstream ss;
			int ii=0;
			for (auto &t: p.get_rpn()) {
				if (ii++>0) ss << ',';
				ss << t.to_str(false);
			}

			std::cout << " Test " << test(
				suite, timer.elapsed_sec(),
				parser_tests[ix].src,
				exp_ss.str().c_str(),
				ss.str().c_str(),
				&ok);
		}

		return ok;
	}
}

#endif /* HILL__TEST__PARSER_HH_INCLUDED */
