#ifndef HILL__TEST__JSON_PARSER_HH_INCLUDED
#define HILL__TEST__JSON_PARSER_HH_INCLUDED

#include "../utils/json_parser.hh"
#include "../utils/junit.hh"

#include "./support.hh"

#include <iostream>
#include <sstream>
#include <memory>

namespace hill::test {

	struct {
		const char *src;
		const char *expected_kind;
		const char *expected_value;
	} json_parser_tests[]={
		{"{}", "OBJECT", nullptr},
		{"[]", "ARRAY", nullptr},
		{"\"test\"", "STRING", "\"test\""},
		{"42", "NUMBER", "42"},
		{"3.14", "NUMBER", "3.14"},
		{"true", "BOOL", "true"},
		{"false", "BOOL", "false"},
		{"{ \"obj1\"  : {\"str\":\"str\"}  , \"arr1\":[true, false, 1.23, {}]}",
			"OBJECT,OBJECT,STRING,END,ARRAY,BOOL,BOOL,NUMBER,OBJECT,END,END",
			"{\"obj1\":{\"str\":\"str\"},\"arr1\":[true,false,1.23,{}]}"},
	};

	const char *json_parser_failing_tests[] = {
		"",
		"{",
		"[",
		"}",
		"]",
		"{{}",
		"{}}",
		"\"test",
		"{2}",
		"tru",
		"fals",
	};

	inline bool json_parser(utils::junit_session &test_session)
	{
		bool ok = true;
		auto suite = test_session.add_suite("Json Parser");

		std::cout << "Json Parser testing:\n";

		for (size_t ix=0; ix<sizeof json_parser_tests /sizeof json_parser_tests[0]; ++ix) {
			utils::timer timer;
			auto src_ss = get_src(json_parser_tests[ix].src);
			auto exp_kind_ss = get_src(json_parser_tests[ix].expected_kind);
			auto exp_value_ss = get_src(json_parser_tests[ix].expected_value);

			if (src_ss.str().empty()) {
				std::cout << "Cannot read source " << utils::color(json_parser_tests[ix].src, utils::ccolor::YELLOW) << '\n';
				ok = false;
				continue;
			}

			auto json = utils::json_parser::parse(src_ss);

			std::cout << " Kind test  " << test(
				suite,
				timer.elapsed_sec(),
				json_parser_tests[ix].src,
				exp_kind_ss.str().c_str(),
				json.has_value() ? json.value()->kind_str().c_str() : "",
				&ok);

			if (json_parser_tests[ix].expected_value) {
				std::cout << " Kind value " << test(
					suite,
					timer.elapsed_sec(),
					json_parser_tests[ix].src,
					exp_value_ss.str().c_str(),
					json.has_value() ? json.value()->stringify().c_str() : "",
					&ok);
			}
		}

		for (size_t ix=0; ix<sizeof json_parser_failing_tests /sizeof json_parser_failing_tests[0]; ++ix) {
			utils::timer timer;
			auto src_ss = get_src(json_parser_failing_tests[ix]);

			auto json = utils::json_parser::parse(src_ss);

			std::cout << " Should fail " << test(
				suite,
				timer.elapsed_sec(),
				json_parser_failing_tests[ix],
				"",
				json.has_value() ? json.value()->stringify().c_str() : "",
				&ok);
		}

		return ok;
	}
}

#endif /* HILL__TEST__JSON_PARSER_HH_INCLUDED */
