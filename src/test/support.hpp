#ifndef TEST__SUPPORT_HPP_INCLUDED
#define TEST__SUPPORT_HPP_INCLUDED

#include "../utils/console.hpp"
#include <string>
#include <sstream>
#include <fstream>
#include <istream>

namespace hill::test {

	inline std::stringstream get_src(const char *src_spec)
	{
		bool isFileTest = src_spec[0] == ':';

		std::stringstream ss;
		if (isFileTest) {
			std::ifstream fstr(src_spec+1, std::ios_base::binary);
			ss << fstr.rdbuf();
		} else {
			ss << src_spec;
		}

		return ss;
	}

	inline std::string test(
		const char *test_name,
		const char *expected,
		const char *actual,
		bool *ok)
	{
		std::stringstream ss;

		ss << utils::escape_string(test_name[0] == ':' ? (test_name +1) : test_name);
		if (!strcmp(expected, actual)) {
			ss << utils::color(" PASSED", utils::ccolor::GREEN) << '\n';
		} else {
			ss << ' '
				<< utils::color("FAILED", utils::ccolor::WHITE, utils::ccolor::RED)
				<< ": Expected: " << utils::escape_string(expected)
				<< " - Actual: " << utils::escape_string(actual)
				<< '\n';
			*ok = false;
		}

		return ss.str();
	}
}

#endif /* TEST__SUPPORT_HPP_INCLUDED */
