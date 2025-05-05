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

total: = 100_000L;
PI: = 3.1415f;

a b;
)");

	hill::token token;
	while (!(token=std::move(hill::get_token(istr))).end()) {
		std::cout << token.str();
	}
}

static void test_parser()
{
	std::istringstream istr("\r\na:=5\r\n\r\n");

	hill::parser parser;

	parser.parse(istr, hill::get_token);
}

struct {
	const char *src_fname;
	uint64_t expected_resulted;
} tests[]={
	//{"tests/initial-assignment.hill", 5},
	//{"tests/add.hill", 7},
	{"tests/sub.hill", 5},
	//{"tests/add-assign.hill", 5},
	//{"tests/post-increment.hill", 0},
	//{"tests/post-decrement.hill", 0},
	//{"tests/pre-increment.hill", 0},
	//{"tests/pre-decrement.hill", 0},
};

static void run_tests()
{
	int ix;

	for (ix = 0; ix<sizeof tests/sizeof tests[0]; ++ix) {
		std::cout << tests[ix].src_fname << ":\n";

		std::ifstream istr(tests[ix].src_fname);
		hill::parser p;
		hill::analyzer a;
		hill::evaluator e;
		hill::hill(istr, hill::get_token, p, a, e);
		//parser.parse(istr);

		//hill(get_token, parser, analyzer, {opt1,opt2}, evaluator);
		//hill(get_token, parser, analyzer, {opt1,opt2}, csharp_code_gen);

		std::cout << '\n';
	}
}

int main(int argc, char *argv[])
{
	//test_lexer();
	//test_parser();

	run_tests();

	return EXIT_SUCCESS;
}
