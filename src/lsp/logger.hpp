#ifndef HILL__LSP__LOGGER_HPP_INCLUDED
#define HILL__LSP__LOGGER_HPP_INCLUDED

#include <chrono>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <thread>

namespace hill::lsp {

	enum log_level: int {
		TRACE,
		INFO,
		WARN,
		ERROR,
		CRITICAL,
	};

	struct logger {
		log_level level = log_level::TRACE;

		void open(const std::filesystem::path &fpath)
		{
			std::filesystem::create_directories(fpath.parent_path());
			ofs.open(fpath, std::ios::binary);
		}

		void writeln(const std::string &msg)
		{
			auto now = std::chrono::system_clock::now();

			std::lock_guard<std::mutex> guard(mutex);

			ofs << std::format("{:%T}", now);
			ofs << " - ";
			ofs << '[' << std::setw(5) << std::right << std::this_thread::get_id() << "] ";
			ofs.write(msg.c_str(), msg.size());
			ofs.put('\n');
			ofs.flush();
		}

		void trace(const std::string &msg) {if (level <= log_level::TRACE) writeln("[TRACE] " + msg);}
		void info(const std::string &msg) {if (level <= log_level::INFO) writeln("[INFO] " + msg);}
		void warn(const std::string &msg) {if (level <= log_level::WARN) writeln("[WARN] " + msg);}
		void error(const std::string &msg) {if (level <= log_level::ERROR) writeln("[ERROR] " + msg);}
		void critical(const std::string &msg) {if (level <= log_level::CRITICAL) writeln("[CRITICAL] " + msg);}

	private:
		std::ofstream ofs;
		std::mutex mutex;
	};
}

#endif /* HILL__LSP__LOGGER_HPP_INCLUDED */
