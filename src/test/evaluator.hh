#ifndef HILL__TEST__EVALUATOR_HH_INCLUDED
#define HILL__TEST__EVALUATOR_HH_INCLUDED

#include "../lexer.hh"
#include "../parser.hh"
#include "../analyzer.hh"
#include "../evaluator.hh"
#include "../hill.hh"
#include "../utils/console.hh"

#include "./support.hh"

#include <sstream>
#include <fstream>
#include <istream>

namespace hill::test {

	struct {
		const char *src;
		//uint64_t expected_resulted;
		const char *expected_type;
		const char *expected_value;
		error_code expected_error_code;
	} evaluator_tests[]={
		{":tests/initial-assignment.hill", "@i32", "5", error_code::NO_ERROR},
		{":tests/add.hill", "@i32", "7", error_code::NO_ERROR},
		{":tests/sub.hill", "@i32", "-1", error_code::NO_ERROR},
		{":tests/tuple.hill", "(@i32,@i32)", "(1,2)", error_code::NO_ERROR},
		{":tests/tuple-diff-types.hill", "(@f64,@i32)", "(1.0,2)", error_code::NO_ERROR},
		{":tests/tuple-in-tuple.hill", "(@i32,(@i32,@i32))", "(1,(2,3))", error_code::NO_ERROR},
		{":tests/tuples-in-tuple.hill", "((@i32,@i32),(@i32,@i32))", "((1,2),(3,4))", error_code::NO_ERROR},
		{"((1,2,3),(4,5))", "((@i32,@i32,@i32),(@i32,@i32))", "((1,2,3),(4,5))", error_code::NO_ERROR},
		{"((1,2),(3,4,5))", "((@i32,@i32),(@i32,@i32,@i32))", "((1,2),(3,4,5))", error_code::NO_ERROR},
		{":tests/pi.hill", "@f64", "3.14", error_code::NO_ERROR},
		{"1+2", "@i32", "3", error_code::NO_ERROR},
		{"3-2", "@i32", "1", error_code::NO_ERROR},
		{"5*3", "@i32", "15", error_code::NO_ERROR},
		{"abs (-1)", "@i32", "1", error_code::NO_ERROR},
		{"abs 1", "@i32", "1", error_code::NO_ERROR},
		{"pow (2, 3)", "@i32", "8", error_code::NO_ERROR},
		{"pow (2.0, 3.0)", "@f64", "8.0", error_code::NO_ERROR},
		{"the_answer", "@i32", "42", error_code::NO_ERROR},
		{"2 |> pow 16", "@i32", "65536", error_code::NO_ERROR},
		{"2 |> pow 8 |> pow 2", "@i32", "65536", error_code::NO_ERROR},
		{"3 |> pow 2 |> div 3 |> pow 3", "@i32", "27", error_code::NO_ERROR},
		{"[1, 2]", "@array(@i32,2)", "[1,2]", error_code::NO_ERROR},
		{"[1.0, 2.0]", "@array(@f64,2)", "[1.0,2.0]", error_code::NO_ERROR},
		{"[1]", "@array(@i32,1)", "[1]", error_code::NO_ERROR},
		{"[(1,2),(3,4)]", "@array((@i32,@i32),2)", "[(1,2),(3,4)]", error_code::NO_ERROR},
		{"[(1.0,2),(3.0,4),(5.0,6)]", "@array((@f64,@i32),3)", "[(1.0,2),(3.0,4),(5.0,6)]", error_code::NO_ERROR},
		{"[(1.0,2),(3.0,4),(5.0,6.0)]", "", "", error_code::ARRAY_ELM_TYPE_MISMATCH},
		//{"a:=1;a=2", "@i32", "2", error_code::NO_ERROR},
		//{"a:=1;a=a+2", "@i32", "3", error_code::NO_ERROR},
		//{"a:=1;b:=10;a=a+2", "@i32", "3", error_code::NO_ERROR},
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

			::hill::value val;
			error_code ec = error_code::NO_ERROR;
			try {
				val = ::hill::hill(src_ss, l, p, a, e);
			} catch (::hill::exception &ex) {
				ec = ex.get_error_code();
			}

			double duration = timer.elapsed_sec();
			if (evaluator_tests[ix].expected_error_code != error_code::NO_ERROR) {
				std::cout << " Error test " << test(
					suite, duration,
					evaluator_tests[ix].src,
					error_code_to_str(evaluator_tests[ix].expected_error_code).c_str(),
					error_code_to_str(ec).c_str(),
					&ok);
			} else {
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
		}

		return ok;
	}
}

#endif /* HILL__TEST__EVALUATOR_HH_INCLUDED */
