#include "lexer.hh"
#include "parser.hh"
#include "analyzer.hh"
#include "evaluator.hh"
#include "hill.hh"

#include "lsp/server.hh"
#include "fmt/formatter.hh"

#include "utils/junit.hh"

#include "test/lexer.hh"
#include "test/parser.hh"
#include "test/analyzer.hh"
#include "test/evaluator.hh"

#include "test/json_parser.hh"

#include <stdlib.h>
#include <string.h>

static int usage(const char *cmd)
{
	std::cerr << "Usage: " << cmd << " [command [argument]\n";
	std::cerr << "Commands:\n";
	std::cerr << " run <file-path> - Evaluate a file and print the result\n";
	std::cerr << " fmt <files/directories> - Run formatter on one or more files/directories\n";
	std::cerr << " lsp - Run language server\n";
	std::cerr << " repl - Start a Read Evaluate Print Loop\n";
	std::cerr << " test <subsystem> - Test the selected sub-system (evaluator)\n";
	std::cerr << "If no command is supplied, all tests will be performed\n";
	return EXIT_FAILURE;
}

int main(int argc, char *argv[])
{
	bool ok = true;

	if (argc>1) {
		if (!strcmp(argv[1], "run")) {
			// TODO: Run file
		} else if (!strcmp(argv[1], "fmt")) {
			::hill::fmt::formatter fmt;
			// TODO: Handle flags
			// TODO: Maybe allow formatting whatever is on stdin and printing it to stdout?
			for (int i=2; i<argc; ++i) {
				fmt.format(argv[i]);
			}
		} else if (!strcmp(argv[1], "lsp")) {
			::hill::lsp::server server;
			server.run();
		} else if (!strcmp(argv[1], "repl")) {
			// TODO: REPL
		} else if (!strcmp(argv[1], "test")) {
			// TODO: Maybe make junit optional?
			// TODO: Maybe allow setting output file
			auto test_session = ::hill::utils::junit_session("Hill unit tests", "test-results.xml");
			if (argc>2) {
				if (!strcmp(argv[2], "lexer")) {ok = ::hill::test::lexer(test_session);}
				else if (!strcmp(argv[2], "parser")) {ok = ::hill::test::parser(test_session);}
				//else if (!strcmp(argv[2], "analyzer")) {/*ok = hill::test::analyzer(test_session); */ }
				else if (!strcmp(argv[2], "evaluator")) {ok = ::hill::test::evaluator(test_session);}
				else if (!strcmp(argv[2], "json_parser")) {ok = ::hill::test::json_parser(test_session);}
				else {return usage(argv[0]);}
			} else {
				if (!::hill::test::lexer(test_session)) ok =false;
				if (!::hill::test::parser(test_session)) ok = false;
				//if (!::hill::test::analyzer(test_session)) ok = false;
				if (!::hill::test::evaluator(test_session)) ok = false;
				if (!::hill::test::json_parser(test_session)) ok = false;
			}
			std::cout << '\n';
			::hill::test::test_report(test_session, std::cout);
		}
	} else {
		auto test_session = ::hill::utils::junit_session("Hill unit tests", "test-results.xml");
		//if (!::hill::test::lexer(test_session)) ok = false;
		//if (!::hill::test::parser(test_session)) ok = false;
		//if (!::hill::test::analyzer(test_session)) ok = false;
		if (!::hill::test::evaluator(test_session)) ok = false;
		if (!::hill::test::json_parser(test_session)) ok = false;
		std::cout << '\n';
		::hill::test::test_report(test_session, std::cout);
	}

	return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
