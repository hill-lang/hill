#include "lexer.hpp"
#include "parser.hpp"
#include "analyzer.hpp"
#include "evaluator.hpp"
#include "hill.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <cstdint>

static void test_lexer()
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

	hill::token token;
	hill::lexer lexer;
	while (!(token=std::move(lexer.get_token(istr))).end()) {
		std::cout << token.to_str() << '\n';
	}
}

static void test_parser()
{
	std::istringstream istr("\r\na:=5\r\n\r\n");

	hill::lexer lexer;
	hill::parser parser;

	parser.parse(istr, lexer);
}

struct {
	const char *src_fname;
	//uint64_t expected_resulted;
	const char *expected_type;
	const char *expected_value;
} tests[]={
	{"tests/initial-assignment.hill", "@i", "5"},
	{"tests/add.hill", "@i", "7"},
	{"tests/sub.hill", "@i", "-1"},
	//{"tests/post-increment.hill", 0},
	//{"tests/post-decrement.hill", 0},
	//{"tests/pre-increment.hill", 0},
	//{"tests/pre-decrement.hill", 0},
	{"tests/tuple.hill", "(@i,@i)", "(1,2)"},
	{"tests/tuple-diff-types.hill", "(@f,@i)", "(1.0,2)"},
	{"tests/tuple-in-tuple.hill", "(@i,(@i,@i))", "(1,(2,3))"},
	{"tests/tuples-in-tuple.hill", "((@i,@i),(@i,@i))", "((1,2),(3,4))"},
	{"tests/pi.hill", "@f", "3.14"},
};

static void run_tests()
{
	int ix;

	for (ix = 0; ix<sizeof tests/sizeof tests[0]; ++ix) {
		std::ifstream istr(tests[ix].src_fname);
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

int main(int argc, char *argv[])
{
	//test_lexer();
	//test_parser();

	run_tests();

	return EXIT_SUCCESS;
}
