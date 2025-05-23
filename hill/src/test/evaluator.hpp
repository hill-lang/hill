#ifndef TEST__EVALUATOR_HPP_INCLUDED
#define TEST__EVALUATOR_HPP_INCLUDED

#include "lexer.hpp"
#include "parser.hpp"
#include "analyzer.hpp"
#include "evaluator.hpp"
#include "hill.hpp"
#include "utils/console.hpp"

#include "test/support.hpp"

#include <sstream>
#include <fstream>
#include <istream>

namespace hill {

	namespace test {

		struct {
			const char *src;
			//uint64_t expected_resulted;
			const char *expected_type;
			const char *expected_value;
		} evaluator_tests[]={
			{":tests/initial-assignment.hill", "@i", "5"},
			{":tests/add.hill", "@i", "7"},
			{":tests/sub.hill", "@i", "-1"},
			{":tests/tuple.hill", "(@i,@i)", "(1,2)"},
			{":tests/tuple-diff-types.hill", "(@f,@i)", "(1.0,2)"},
			{":tests/tuple-in-tuple.hill", "(@i,(@i,@i))", "(1,(2,3))"},
			{":tests/tuples-in-tuple.hill", "((@i,@i),(@i,@i))", "((1,2),(3,4))"},
			{":tests/pi.hill", "@f", "3.14"},
			{"1+2", "@i", "3"},
			{"3-2", "@i", "1"},
			{"5*3", "@i", "15"},
		};

		inline bool evaluator()
		{
			std::cout << "Evaluator testing:\n";

			bool ok = true;

			for (size_t ix = 0; ix<sizeof evaluator_tests/sizeof evaluator_tests[0]; ++ix) {
				auto src_ss = get_src(evaluator_tests[ix].src);
				auto exp_type_ss = get_src(evaluator_tests[ix].expected_type);
				auto exp_value_ss = get_src(evaluator_tests[ix].expected_value);

				if (src_ss.str().empty()) {
					std::cerr << "Cannot read source " << utils::setcolor(utils::console_color::YELLOW) << evaluator_tests[ix].src << utils::resetcolor() << '\n';
					continue;
				}

				::hill::lexer l;
				::hill::parser p;
				::hill::analyzer a;
				::hill::evaluator e;
				auto val = ::hill::hill(src_ss, l, p, a, e);

				std::cout << " Type test  " << (evaluator_tests[ix].src[0]==':' ? (evaluator_tests[ix].src+1) : evaluator_tests[ix].src);
				if (!strcmp(exp_type_ss.str().c_str(), val.ts.to_str().c_str())) {
					std::cout << utils::setcolor(utils::console_color::GREEN) << " PASSED\n" << utils::resetcolor();
				} else {
					std::cout << ' '
						<< utils::setcolor(utils::console_color::WHITE)
						<< utils::setbgcolor(utils::console_color::RED)
						<< "FAILED"
						<< utils::resetcolor()
						<< ": Expected: " << exp_type_ss.str().c_str()
						<< " - Actual: " << val.to_str() << '\n';
					ok = false;
				}

				std::cout << " Value test " << (evaluator_tests[ix].src[0]==':' ? (evaluator_tests[ix].src+1) : evaluator_tests[ix].src);
				if (!strcmp(exp_value_ss.str().c_str(), val.to_str().c_str())) {
					std::cout << utils::setcolor(utils::console_color::GREEN) << " PASSED\n" << utils::resetcolor();
				} else {
					std::cout << ' '
						<< utils::setcolor(utils::console_color::WHITE)
						<< utils::setbgcolor(utils::console_color::RED)
						<< "FAILED"
						<< utils::resetcolor()
						<< ": Expected: " << exp_value_ss.str().c_str()
						<< " - Actual: " << val.to_str() << '\n';
				}
			}

			return ok;
		}
	}
}

#endif /* TEST__EVALUATOR_HPP_INCLUDED */
