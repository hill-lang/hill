#ifndef HILL__UTILS__JUNIT_HPP_INCLUDED
#define HILL__UTILS__JUNIT_HPP_INCLUDED

// https://github.com/testmoapp/junitxml

#include "xml_writer.hpp"

#include <chrono>
#include <format>
#include <filesystem>
#include <string>
#include <vector>
#include <sstream>
#include <memory>

namespace hill::utils {

	enum class junit_test_failure_type {
		NONE,
		ASSERTION,
		//ARITHMETIC_ERROR, // e.g. Division by zero (This is an error, not an assertion)
	};

	constexpr const char *junit_test_failure_type_str(junit_test_failure_type type)
	{
		switch (type) {
		case junit_test_failure_type::ASSERTION: return "AssertionError";
		default: return "UNKNOWN";
		}
	}

	struct junit_test_failure {
		junit_test_failure() : type(junit_test_failure_type::NONE) {}

		std::string message;
		junit_test_failure_type type;
	};

	struct junit_test_case {
		junit_test_case(const std::string &name, const std::string &classname)
			: name(name), classname(classname), time(0.0)
		{
		}

		std::string name;
		std::string classname;
		double time;
		size_t assertion_count = 1; // TODO: Maybe allow multiple assertions per case?
		junit_test_failure failure;
		//std::vector<junit_test_failure> failures;

		// bool skipped;
		// std::string skipped_reason; // Optional
	};

	struct junit_test_suite {
		junit_test_suite(const std::string &name)
			: name(name)
		{
			timestamp = std::chrono::system_clock::now();
		}

		std::string name;
		std::chrono::system_clock::time_point timestamp;
		std::vector<std::shared_ptr<junit_test_suite>> test_suites;
		std::vector<std::shared_ptr<junit_test_case>> test_cases;

		std::shared_ptr<junit_test_suite> add_suite(const std::string &suite_name)
		{
			auto suite = std::make_shared<junit_test_suite>(suite_name);
			test_suites.push_back(suite);
			return suite;
		}

		std::shared_ptr<junit_test_case> add_case(const std::string &case_name)
		{
			auto test_case = std::make_shared<junit_test_case>(case_name, this->name);
			test_cases.push_back(test_case);
			return test_case;
		}

		double time() const
		{
			double t = 0.0;
			for (auto &suite : test_suites) {
				t += suite->time();
			}
			for (auto &test_case : test_cases) {
				t += test_case->time;
			}
			return t;
		}

		size_t test_count() const
		{
			size_t cnt = 0;
			for (auto &suite : test_suites) {
				cnt += suite->test_count();
			}
			cnt += test_cases.size();
			return cnt;
		}

		size_t assertion_count() const
		{
			size_t cnt = 0;
			for (auto &suite : test_suites) {
				cnt += suite->assertion_count();
			}
			for (auto &test_case : test_cases) {
				cnt += test_case->assertion_count;
			}
			return cnt;
		}

		size_t failed_count() const
		{
			size_t cnt = 0;
			for (auto &suite : test_suites) {
				cnt += suite->failed_count();
			}
			for (auto &test_case : test_cases) {
				if (test_case->failure.type != junit_test_failure_type::NONE) {
					++cnt;
				}
			}
			return cnt;
		}
	};

	struct junit_session {
		explicit junit_session(const std::string &name, const std::filesystem::path &fpath)
			: name(name)
		{
			xml = std::make_shared<xml_writer>(fpath);
			timestamp = std::chrono::system_clock::now();
		}
		explicit junit_session(const std::string &name, const std::shared_ptr<std::ostream> &os)
			: name(name)
		{
			xml = std::make_shared<xml_writer>(os);
			timestamp = std::chrono::system_clock::now();
		}

		~junit_session()
		{
			if (this->xml) {
				write(*this, xml);
			}
		}

		std::string name;
		std::chrono::system_clock::time_point timestamp;
		std::vector<std::shared_ptr<junit_test_suite>> test_suites;
		std::shared_ptr<xml_writer> xml;

		std::shared_ptr<junit_test_suite> add_suite(const std::string &suite_name)
		{
			auto suite = std::make_shared<junit_test_suite>(suite_name);
			test_suites.push_back(suite);
			return suite;
		}

	private:
		double time() const
		{
			double t = 0.0;
			for (auto &suite : test_suites) {
				t += suite->time();
			}
			return t;
		}

		size_t test_count() const
		{
			size_t cnt = 0;
			for (auto &suite : test_suites) {
				cnt += suite->test_count();
			}
			return cnt;
		}

		size_t assertion_count() const
		{
			size_t cnt = 0;
			for (auto &suite : test_suites) {
				cnt += suite->assertion_count();
			}
			return cnt;
		}

		size_t failed_count() const
		{
			size_t cnt = 0;
			for (auto &suite : test_suites) {
				cnt += suite->failed_count();
			}
			return cnt;
		}

	private:
		static void write_case(
			const std::shared_ptr<junit_test_case> &test_case,
			const std::shared_ptr<xml_writer> &xml)
		{
			xml->push_section("testcase");

			xml->set_attribute("name", test_case->name);
			xml->set_attribute("classname", test_case->classname);

			auto timestr = std::to_string(test_case->time);
			xml->set_attribute("time", timestr);

			auto assertioncntstr = std::to_string(test_case->assertion_count);
			xml->set_attribute("assertions", assertioncntstr);

			if (test_case->failure.type != junit_test_failure_type::NONE) {
				xml->push_section("failure");
				xml->set_attribute("message", test_case->failure.message);
				xml->set_attribute("type", junit_test_failure_type_str(test_case->failure.type));
				xml->set_self_closing(true);
				xml->pop_section();
			} else {
				xml->set_self_closing(true);
			}

			xml->pop_section();
		}

		static void write_suite(
			const std::shared_ptr<junit_test_suite> &suite,
			const std::shared_ptr<xml_writer> &xml)
		{
			xml->push_section("testsuite");

			xml->set_attribute("name", suite->name);

			auto testcntstr = std::to_string(suite->test_count());
			xml->set_attribute("tests", testcntstr);

			auto assertioncntstr = std::to_string(suite->assertion_count());
			xml->set_attribute("assertions", assertioncntstr);

			auto failedcntstr = std::to_string(suite->failed_count());
			xml->set_attribute("failures", failedcntstr);

			// TODO: Do we are for supporting this?
			xml->set_attribute("errors", "0");
			xml->set_attribute("skipped", "0");

			auto timestr = std::to_string(suite->time());
			xml->set_attribute("time", timestr);

			xml->set_attribute("timestamp", std::format("{:%FT%TZ}", suite->timestamp));

			for (const auto &suite : suite->test_suites) {
				write_suite(suite, xml);
			}

			for (const auto &test_case : suite->test_cases) {
				write_case(test_case, xml);
			}

			xml->pop_section();
		}

		static void write(
			const junit_session &session,
			const std::shared_ptr<xml_writer> &xml)
		{
			xml->push_section("testsuites");

			xml->set_attribute("name", session.name);

			auto testcntstr = std::to_string(session.test_count());
			xml->set_attribute("tests", testcntstr);

			auto assertioncntstr = std::to_string(session.assertion_count());
			xml->set_attribute("assertions", assertioncntstr);

			auto failedcntstr = std::to_string(session.failed_count());
			xml->set_attribute("failures", failedcntstr);

			// TODO: Do we are for supporting this?
			xml->set_attribute("errors", "0");
			xml->set_attribute("skipped", "0");

			auto timestr = std::to_string(session.time());
			xml->set_attribute("time", timestr);

			xml->set_attribute("timestamp", std::format("{:%FT%TZ}", session.timestamp));

			for (const auto &suite : session.test_suites) {
				write_suite(suite, xml);
			}

			xml->pop_section();
		}
	};
}

#endif /* HILL__UTILS__JUNIT_HPP_INCLUDED */
