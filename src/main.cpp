#include "lexer.hpp"
#include "parser.hpp"
#include "analyzer.hpp"
#include "evaluator.hpp"
#include "hill.hpp"

#include "utils/junit.hpp"
#include "test/lexer.hpp"
#include "test/parser.hpp"
#include "test/analyzer.hpp"
#include "test/evaluator.hpp"

#include <stdlib.h>
#include <string.h>

static int usage(const char *cmd)
{
	std::cerr << "Usage: " << cmd << " [command [argument]\n";
	std::cerr << "Commands:\n";
	std::cerr << " run <file-path> - Evaluate a file and print the result\n";
	std::cerr << " repl - Start a Read Evaluate Print Loop\n";
	std::cerr << " test <subsystem> - Test the selected sub-system (evaluator)\n";
	std::cerr << "If no command is supplied, all tests will be performed\n";
	return EXIT_FAILURE;
}

int main(int argc, char *argv[])
{
	// TODO: Don't do this later. But usefull for testing
	std::filesystem::create_directories("./tmp/");

	bool ok = true;

	if (argc>1) {
		if (!strcmp(argv[1], "run")) {
			// TODO: Run file
		} else if (!strcmp(argv[1], "repl")) {
			// TODO: REPL
		} else if (!strcmp(argv[1], "test")) {
			// TODO: Maybe make junit optional?
			// TODO: Maybe allow setting output file
			auto test_session = hill::utils::junit_session("Hill unit tests", "test-results.xml");
			if (argc>2) {
				if (!strcmp(argv[2], "lexer")) {
					ok = hill::test::lexer(test_session);
				} else if (!strcmp(argv[2], "parser")) {
					ok = hill::test::parser(test_session);
				} else if (!strcmp(argv[2], "analyzer")) {
					// ok = hill::test::analyzer(test_session);
				} else if (!strcmp(argv[2], "evaluator")) {
					ok = hill::test::evaluator(test_session);
				} else {
					return usage(argv[0]);
				}
			} else {
				if (!hill::test::lexer(test_session)) ok =false;
				if (!hill::test::parser(test_session)) ok = false;
				//if (!hill::test::analyzer(test_session)) ok = false;
				if (!hill::test::evaluator(test_session)) ok = false;
			}
		}
	} else {
		auto test_session = hill::utils::junit_session("Hill unit tests", "test-results.xml");
		if (!hill::test::lexer(test_session)) ok = false;
		if (!hill::test::parser(test_session)) ok = false;
		//if (!hill::test::analyzer(test_session)) ok = false;
		if (!hill::test::evaluator(test_session)) ok = false;
	}

	return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
