#ifndef HILL__TEST__SUPPORT_HPP_INCLUDED
#define HILL__TEST__SUPPORT_HPP_INCLUDED

#include "../utils/console.hpp"
#include "../utils/junit.hpp"
#include "../utils/timer.hpp"

#include <filesystem>
#include <string>
#include <sstream>
#include <fstream>
#include <istream>
#include <memory>

namespace hill::test {

	inline std::stringstream get_src(const char *src_spec)
	{
		std::stringstream ss;

		if (src_spec) {
			bool isFileTest = src_spec[0] == ':';

			if (isFileTest) {
				std::ifstream fstr(src_spec+1, std::ios_base::binary);
				ss << fstr.rdbuf();
			} else {
				ss << src_spec;
			}
		}

		return ss;
	}

	inline std::string test(
		const std::shared_ptr<utils::junit_test_suite> &suite,
		double duration,
		const char *test_name,
		const char *expected,
		const char *actual,
		bool *ok)
	{
		std::stringstream ss;

		auto name = test_name[0] == ':' ? (test_name + 1) : test_name;
		auto test_case = suite->add_case(name);
		test_case->time = duration;

		ss << utils::escape_string(name);
		if (!strcmp(expected, actual)) {
			ss << utils::color(" PASSED", utils::ccolor::GREEN) << '\n';
		} else {
			std::stringstream fail_ss;
			fail_ss << "Expected: " << utils::escape_string(expected)
				<< " - Actual: " << utils::escape_string(actual);
			test_case->failure.message = fail_ss.str();
			test_case->failure.type = utils::junit_test_failure_type::ASSERTION;

			ss << ' '
				<< utils::color("FAILED", utils::ccolor::WHITE, utils::ccolor::RED)
				<< ": " << fail_ss.str() << '\n';

			*ok = false;
		}

		return ss.str();
	}

	inline void test_report(const utils::junit_session &s, std::ostream &os)
	{
		os << std::setw(11) << std::right << "Session" << "  " << s.name << '\n';
		os << std::setw(11) << std::right << "Tests" << " ";
		if (s.failed_count()>0) {
			os << " " << ::hill::utils::color(std::to_string(s.failed_count()) + " failed", ::hill::utils::ccolor::RED);
		}
		if (s.passed_count()>0) {
			os << " " << ::hill::utils::color(std::to_string(s.passed_count()) + " passed", ::hill::utils::ccolor::GREEN);
		}
		os << " " << s.test_count() << " total\n";

		os << std::setw(11) << std::right << "Duration" << "  " << std::to_string(s.time()) << " ms\n";
	}
}

#endif /* HILL__TEST__SUPPORT_HPP_INCLUDED */
