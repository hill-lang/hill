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

	inline std::string passed()
	{
		return utils::color(" PASSED", utils::ccolor::GREEN);
	}

	inline std::string failed(const char *expected, const char *actual)
	{
		std::stringstream ss;

		ss << ' '
			<< utils::color("FAILED", utils::ccolor::WHITE, utils::ccolor::RED)
			<< ": Expected: " << utils::escape_string(expected)
			<< " - Actual: " << utils::escape_string(actual);

		return ss.str();
	}
}

#endif /* TEST__SUPPORT_HPP_INCLUDED */
