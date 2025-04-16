
#include "lexer.hpp"
#include "parser.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>

void test_lexer()
{
	std::ifstream istr("lexer.hill");

	auto token = std::shared_ptr<hill::token>();
	while (!(token=hill::get_token(istr))->end()) {
		std::cout << token->str();
	}
}

void test_parser()
{
	std::ifstream istr("parser.hill");

	hill::parser parser;

	parser.parse(istr);
}

int main(int argc, char *argv[])
{
	//test_lexer();
	test_parser();

	return EXIT_SUCCESS;
}
