#ifndef HILL__TEST__JSON_PARSER_HPP_INCLUDED
#define HILL__TEST__JSON_PARSER_HPP_INCLUDED

#include "support.hpp"

#include "../utils/json_parser.hpp"
#include "../utils/junit.hpp"

#include <iostream>
#include <sstream>
#include <memory>

namespace hill::test {

	struct {
		const char *src;
		const char *expected_kind;
		const char *expected_value;
	} json_parse_kind_tests[]={
		{"{}", "OBJECT"},
		{"[]", "ARRAY"},
		{"\"test\"", "STRING", "\"test\""},
		{"42", "NUMBER", "42"},
		{"3.14", "NUMBER", "3.14"},
		{"true", "BOOL", "true"},
		{"false", "BOOL", "false"},
		{"{ \"obj1\"  : {\"str\":\"str\"}  , \"arr1\":[true, false, 1.23, {}]}",
			"OBJECT,OBJECT,STRING,END,ARRAY,BOOL,BOOL,NUMBER,OBJECT,END,END",
			"{\"obj1\":{\"str\":\"str\"},\"arr1\":[true,false,1.23,{}]}"},
	};

	inline bool json_parser(utils::junit_session &test_session)
	{
		bool ok = true;
		auto suite = test_session.add_suite("Json Parser");

		std::cout << "Json Parser testing:\n";

		for (size_t ix=0; ix<sizeof json_parse_kind_tests /sizeof json_parse_kind_tests[0]; ++ix) {
			utils::timer timer;
			auto src_ss = get_src(json_parse_kind_tests[ix].src);
			auto exp_kind_ss = get_src(json_parse_kind_tests[ix].expected_kind);
			auto exp_value_ss = get_src(json_parse_kind_tests[ix].expected_value);

			if (src_ss.str().empty()) {
				std::cout << "Cannot read source " << utils::color(json_parse_kind_tests[ix].src, utils::ccolor::YELLOW) << '\n';
				ok = false;
				continue;
			}

			utils::json_parser parser;
			auto json = parser.parse(src_ss);

			std::cout << " Kind test  " << test(
				suite,
				timer.elapsed_sec(),
				json_parse_kind_tests[ix].src,
				exp_kind_ss.str().c_str(),
				json->kind_str().c_str(),
				&ok);

			if (json_parse_kind_tests[ix].expected_value) {
				std::cout << " Kind value " << test(
					suite,
					timer.elapsed_sec(),
					json_parse_kind_tests[ix].src,
					exp_value_ss.str().c_str(),
					json->to_str().c_str(),
					&ok);
			}
		}

		return ok;
	}
}

#endif /* HILL__TEST__JSON_PARSER_HPP_INCLUDED */
