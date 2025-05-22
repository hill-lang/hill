#ifndef TEST_HPP_INCLUDED
#define TEST_HPP_INCLUDED

#include "lexer.hpp"
#include "parser.hpp"
#include "analyzer.hpp"
#include "evaluator.hpp"
#include "hill.hpp"

#include <sstream>
#include <fstream>
#include <istream>

namespace hill {

	namespace test {

		struct {
			const char *src_fname;
			//uint64_t expected_resulted;
			const char *expected_type;
			const char *expected_value;
		} tests[]={
			{":tests/initial-assignment.hill", "@i", "5"},
			{":tests/add.hill", "@i", "7"},
			{":tests/sub.hill", "@i", "-1"},
			{":tests/tuple.hill", "(@i,@i)", "(1,2)"},
			{":tests/tuple-diff-types.hill", "(@f,@i)", "(1.0,2)"},
			{":tests/tuple-in-tuple.hill", "(@i,(@i,@i))", "(1,(2,3))"},
			{":tests/tuples-in-tuple.hill", "((@i,@i),(@i,@i))", "((1,2),(3,4))"},
			{":tests/pi.hill", "@f", "3.14"},
			{"1+2", "@i", "3"}
		};

		static void evaluator()
		{
			int ix;

			for (ix = 0; ix<sizeof tests/sizeof tests[0]; ++ix) {
				std::stringstream istr;
				if (tests[ix].src_fname[0]==':') {
					std::ifstream fstr(tests[ix].src_fname+1);
					std::copy(std::istream_iterator<char>(fstr), std::istream_iterator<char>(), std::ostreambuf_iterator(istr));
				} else {
					istr << tests[ix].src_fname;
				}
				hill::lexer l;
				hill::parser p;
				hill::analyzer a;
				hill::evaluator e;
				auto val = hill::hill(istr, l, p, a, e);

				std::cout << "Type test " << tests[ix].src_fname;
				if (!strcmp(tests[ix].expected_type, val.ts.to_str().c_str())) {
					std::cout << " PASSED\n";
				} else {
					std::cout << " FAILED: Expected: " << tests[ix].expected_type << " - Actual: " << val.ts.to_str() << '\n';
				}

				std::cout << "Value test " << tests[ix].src_fname;
				if (!strcmp(tests[ix].expected_value, val.to_str().c_str())) {
					std::cout << " PASSED\n";
				} else {
					std::cout << " FAILED: Expected: " << tests[ix].expected_value << " - Actual: " << val.to_str() << '\n';
				}
			}
		}
	}
}


#endif /* TEST_HPP_INCLUDED */
