#include "lexer.hpp"
#include "parser.hpp"
#include "analyzer.hpp"
#include "evaluator.hpp"
#include "hill.hpp"

#include "test/lexer.hpp"
#include "test/parser.hpp"
#include "test/analyzer.hpp"
#include "test/evaluator.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <cstdint>

static void test_parser()
{
	std::istringstream istr("\r\na:=5\r\n\r\n");

	hill::lexer lexer;
	hill::parser parser;

	parser.parse(istr, lexer);
}

void fatal(const char *msg)
{
	std::cerr << "Error: " << msg << '\n';
	exit(EXIT_FAILURE);
}

int usage(const char *cmd)
{
	std::cerr << "Usage: " << cmd << " [command [argument]\n";
	std::cerr << "Commands:\n";
	std::cerr << " run <file-path> - Evaluate a file and print the result\n";
	std::cerr << " repl - Start a read evaluate print loop\n";
	std::cerr << " test <subsystem> - Test the selected sub-system (evaluator)\n";
	std::cerr << "If no command is supplied, all tests will be performed\n";
	return EXIT_FAILURE;
}

int main(int argc, char *argv[])
{
	if (argc>1) {
		if (!strcmp(argv[1], "run")) {
			// TODO: Run file
		} else if (!strcmp(argv[1], "repl")) {
			// TODO: REPL
		} else if (!strcmp(argv[1], "test")) {
			if (argc>2) {
				if (!strcmp(argv[2], "lexer")) {
					hill::test::lexer();
				} else if (!strcmp(argv[2], "parser")) {
					// TODO:
				} else if (!strcmp(argv[2], "analyzer")) {
					// TODO:
				} else if (!strcmp(argv[2], "evaluator")) {
					hill::test::evaluator();
				} else {
					return usage(argv[0]);
				}
			} else {
				hill::test::lexer();
				hill::test::parser();
				//hill::test::analyzer();
				hill::test::evaluator();
			}
		}
	} else {
		hill::test::lexer();
		hill::test::parser();
		//hill::test::analyzer();
		hill::test::evaluator();
	}

	return EXIT_SUCCESS;
}
