#ifndef HILL__TEST__EVALUATOR_HPP_INCLUDED
#define HILL__TEST__EVALUATOR_HPP_INCLUDED

#include "../lexer.hpp"
#include "../parser.hpp"
#include "../analyzer.hpp"
#include "../evaluator.hpp"
#include "../hill.hpp"
#include "../utils/console.hpp"

#include "support.hpp"

#include <sstream>
#include <fstream>
#include <istream>

namespace hill::test {

	struct {
		const char *src;
		//uint64_t expected_resulted;
		const char *expected_type;
		const char *expected_value;
	} evaluator_tests[]={
		{":tests/initial-assignment.hill", "@i32", "5"},
		{":tests/add.hill", "@i32", "7"},
		{":tests/sub.hill", "@i32", "-1"},
		{":tests/tuple.hill", "(@i32,@i32)", "(1,2)"},
		{":tests/tuple-diff-types.hill", "(@f64,@i32)", "(1.0,2)"},
		{":tests/tuple-in-tuple.hill", "(@i32,(@i32,@i32))", "(1,(2,3))"},
		{":tests/tuples-in-tuple.hill", "((@i32,@i32),(@i32,@i32))", "((1,2),(3,4))"},
		{"((1,2,3),(4,5))", "((@i32,@i32,@i32),(@i32,@i32))", "((1,2,3),(4,5))"},
		{"((1,2),(3,4,5))", "((@i32,@i32),(@i32,@i32,@i32))", "((1,2),(3,4,5))"},
		{":tests/pi.hill", "@f64", "3.14"},
		{"1+2", "@i32", "3"},
		{"3-2", "@i32", "1"},
		{"5*3", "@i32", "15"},
		{"abs (-1)", "@i32", "1"},
		{"abs 1", "@i32", "1"},
		{"pow (2, 3)", "@i32", "8"},
		{"pow (2.0, 3.0)", "@f64", "8.0"},
		{"the_answer", "@i32", "42"},
		//{"a:=1;a=2", "@i32", "2"},
		//{"a:=1;a=a+2", "@i32", "3"},
		//{"a:=1;b:=10;a=a+2", "@i32", "3"},
	};

	inline bool evaluator(utils::junit_session &test_session)
	{
		auto suite = test_session.add_suite("Test.Evaluator");

		bool ok = true;

		std::cout << "Evaluator testing:\n";

		for (size_t ix=0; ix<sizeof evaluator_tests/sizeof evaluator_tests[0]; ++ix) {
			utils::timer timer;
			auto src_ss = get_src(evaluator_tests[ix].src);
			auto exp_type_ss = get_src(evaluator_tests[ix].expected_type);
			auto exp_value_ss = get_src(evaluator_tests[ix].expected_value);

			if (src_ss.str().empty()) {
				std::cout << "Cannot read source " << utils::color(evaluator_tests[ix].src, utils::ccolor::YELLOW) << '\n';
				ok = false;
				continue;
			}

			::hill::lexer l;
			::hill::parser p;
			::hill::analyzer a;
			::hill::evaluator e;
			auto val = ::hill::hill(src_ss, l, p, a, e);

			double duration = timer.elapsed_sec();
			std::cout << " Type test  " << test(
				suite, duration,
				evaluator_tests[ix].src,
				exp_type_ss.str().c_str(),
				val.ts.to_str().c_str(),
				&ok);
			std::cout << " Value test " << test(
				suite, duration,
				evaluator_tests[ix].src,
				exp_value_ss.str().c_str(),
				val.to_str().c_str(),
				&ok);
		}

		return ok;
	}
}

#endif /* HILL__TEST__EVALUATOR_HPP_INCLUDED */
