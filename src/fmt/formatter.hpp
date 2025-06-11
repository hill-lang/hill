#ifndef HILL__FMT__FORMATTER_HPP_INCLUDED
#define HILL__FMT__FORMATTER_HPP_INCLUDED

#include "../utils/console.hpp"
#include "../utils/thread_pool.hpp"
#include "../utils/timer.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <istream>
#include <iterator>
#include <mutex>
#include <ostream>
#include <sstream>
#include <stdlib.h>
#include <string>

namespace hill::fmt {

	struct formatter {
		formatter()
		{
			thread_pool.start();
		}

		~formatter()
		{
			thread_pool.stop();
			thread_pool.join();
		}

		static std::string format(const std::istream &istr)
		{
			std::stringstream ss;

			ss << istr.rdbuf();

			return ss.str();
		}

		void format(const std::filesystem::path &path)
		{
			if (!std::filesystem::exists(path)) {
				return;
			}

			if (std::filesystem::is_directory(path)) {
				for (const auto &entry : std::filesystem::directory_iterator(path)) {
					if (std::filesystem::is_directory(entry)) {
						format(entry.path());
					} else {
						thread_pool.queue_job([this, entry] {format(entry.path());});
					}
				}
			} else {
				if (path.extension() != ".hill") {
					return;
				}

				utils::timer timer;

				std::ifstream ifstr(path, std::ios::in | std::ios::binary);
				auto result = formatter::format(ifstr);
				ifstr.close();

				std::ofstream ofstr(path, std::ios::out | std::ios::binary);
				ofstr.write(result.c_str(), result.size());
				ofstr.close();

				bool changed = rand() > RAND_MAX / 2;

				std::stringstream ss;
				ss << (changed ? path.string() : utils::color(path.string(), utils::ccolor::BRIGHT_BLACK));
				ss << " ";
				ss << (int)timer.elapsed_ms();
				ss << "ms";
				if (!changed) {
					ss << utils::color(" (Unchanged)", utils::ccolor::BRIGHT_BLACK);
				}
				ss << "\n";
				log(ss.str());
			}
		}

	private:
		void log(const std::string &msg)
		{
			std::lock_guard<std::mutex> guard(log_mutex);
			std::cout << msg;
		}

	private:
		std::mutex log_mutex;
		utils::thread_pool thread_pool;
	};
}

#endif /* HILL__FMT__FORMATTER_HPP_INCLUDED */
