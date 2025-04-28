#include "lexer.hpp"
#include "parser.hpp"

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

	parser.parse(istr);
}

struct {
	const char *src_fname;
	uint64_t expected_resulted;
} tests[]={
	{"tests/initial-assignment.hill", 5},
	{"tests/post-increment.hill", 0},
	{"tests/post-decrement.hill", 0},
	{"tests/pre-increment.hill", 0},
	{"tests/pre-decrement.hill", 0},
};

static void run_tests()
{
	int ix;

	for (ix = 0; ix<sizeof tests/sizeof tests[0]; ++ix) {
		std::ifstream istr(tests[ix].src_fname);
		hill::parser parser;

		parser.parse(istr);
	}
}

int main(int argc, char *argv[])
{
	//test_lexer();
	//test_parser();

	run_tests();

	return EXIT_SUCCESS;
}
