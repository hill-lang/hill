#ifndef TEST__PARSER_HPP_INCLUDED
#define TEST__PARSER_HPP_INCLUDED

#include "lexer.hpp"

namespace hill {

	namespace test {

		struct {
			const char *src;
			const char *expected;
		} parser_tests[]={
			{"1", "NUM(1),END()"},
			{"a", "NAME(a),END()"},
			{"2 + 3", "NUM(2),NUM(3),OP_PLUS(+):Binary,END()"},
		};

		inline void parser()
		{
			std::cout << "Parser testing:\n";

			int ix;

			for (ix = 0; ix<sizeof parser_tests/sizeof parser_tests[0]; ++ix) {
				auto src_ss = get_src(parser_tests[ix].src);
				auto exp_ss = get_src(parser_tests[ix].expected);

				if (src_ss.str().empty()) {
					std::cerr << "Cannot read source " << utils::setcolor(utils::console_color::YELLOW) << parser_tests[ix].src << utils::resetcolor() << '\n';
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

				std::cout << "Test  " << utils::escape_string(parser_tests[ix].src[0]==':' ? (parser_tests[ix].src+1) : parser_tests[ix].src);
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
				}
			}
		}
	}
}

#endif /* TEST__PARSER_HPP_INCLUDED */
