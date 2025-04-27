#include "lexer.hpp"
#include "parser.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>

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

int main(int argc, char *argv[])
{
	//test_lexer();
	test_parser();

	return EXIT_SUCCESS;
}
