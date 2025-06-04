#ifndef HILL__TEST__JSON_WRITER_HPP_INCLUDED
#define HILL__TEST__JSON_WRITER_HPP_INCLUDED

#include "support.hpp"
#include "../utils/json_writer.hpp"
#include "../utils/junit.hpp"

#include <iostream>
#include <sstream>
#include <memory>

namespace hill::test {

	namespace json_writer_tests {

		/*inline bool template_works(std::shared_ptr<utils::junit_test_suite> &suite)
		{
			utils::timer timer;

			auto oss = std::make_shared<std::ostringstream>();
			hill::utils::json_writer json(oss);

			json.close();

			auto actual = oss->str();
			bool ok = true;
			std::cout  << ' '<< test(
				suite,
				timer.elapsed_sec(),
				"Name",
				"Expected",
				actual.c_str(),
				&ok);

			return ok;
		}*/

		inline bool base_types_works(std::shared_ptr<utils::junit_test_suite> &suite)
		{
			utils::timer timer;

			auto oss = std::make_shared<std::ostringstream>();
			hill::utils::json_writer json(oss);

			json.obj_bool("bool1", true);
			json.obj_bool("bool2", false);
			json.obj_f32("f32", 42.65f);
			json.obj_i32("i32", 123456);
			json.obj_str("str", "hello world");
			json.close();

			auto actual = oss->str();
			bool ok = true;
			std::cout << ' ' << test(
				suite,
				timer.elapsed_sec(),
				"Base types works",
				"{\"bool1\":true,\"bool2\":false,\"f32\":42.65,\"i32\":123456,\"str\":\"hello world\"}",
				actual.c_str(),
				&ok);

			return ok;
		}

		inline bool nested_obj_works(std::shared_ptr<utils::junit_test_suite> &suite)
		{
			utils::timer timer;

			auto oss = std::make_shared<std::ostringstream>();
			hill::utils::json_writer json(oss);
			json.obj("obj1");
			json.obj("obj2");
			json.pop();
			json.obj("obj3");
			json.close();

			auto actual = oss->str();
			bool ok = true;
			std::cout << ' ' << test(
				suite,
				timer.elapsed_sec(),
				"Nested ojects works",
				"{\"obj1\":{\"obj2\":{},\"obj3\":{}}}",
				actual.c_str(),
				&ok);

			return ok;
		}

		inline bool array_works(std::shared_ptr<utils::junit_test_suite> &suite)
		{
			utils::timer timer;

			auto oss = std::make_shared<std::ostringstream>();
			hill::utils::json_writer json(oss);
			json.arr("arr1");
			json.arr_bool(true);
			json.arr_i32(32);
			json.arr_f32(42.34f);
			json.arr_str("hello world");
			json.close();

			auto actual = oss->str();
			bool ok = true;
			std::cout << ' ' << test(
				suite,
				timer.elapsed_sec(),
				"Array works",
				"{\"arr1\":[true,32,42.34,\"hello world\"]}",
				actual.c_str(),
				&ok);

			return ok;
		}

		inline bool nested_arrays_works(std::shared_ptr<utils::junit_test_suite> &suite)
		{
			utils::timer timer;

			auto oss = std::make_shared<std::ostringstream>();
			hill::utils::json_writer json(oss);
			json.arr();
			json.arr();
			json.arr_bool(true);
			json.arr_i32(32);
			json.pop();
			json.arr_f32(42.34f);
			json.arr();
			json.arr_str("hello world");
			json.close();

			auto actual = oss->str();
			bool ok = true;
			std::cout << ' ' << test(
				suite,
				timer.elapsed_sec(),
				"Nested arrays works",
				"[[true,32],42.34,[\"hello world\"]]",
				actual.c_str(),
				&ok);

			return ok;
		}
	};

	inline bool json_writer(utils::junit_session &test_session)
	{
		bool ok = true;
		auto suite = test_session.add_suite("Json Writer");

		std::cout << "Json Writer testing:\n";

		if (!json_writer_tests::base_types_works(suite)) ok = false;
		if (!json_writer_tests::nested_obj_works(suite)) ok = false;
		if (!json_writer_tests::array_works(suite)) ok = false;
		if (!json_writer_tests::nested_arrays_works(suite)) ok = false;

		return ok;
	}
}

#endif /* HILL__TEST__JSON_WRITER_HPP_INCLUDED */
