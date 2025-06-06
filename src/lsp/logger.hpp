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

		static void set_log_level(log_level level)
		{
			auto &l = get();
			l.level = level;
		}

		static void open(const std::filesystem::path &fpath)
		{
			std::filesystem::create_directories(fpath.parent_path());
			get().ofs.open(fpath, std::ios::binary);
		}

		static void writeln(const std::string &msg)
		{
			auto &l = get();

			auto now = std::chrono::system_clock::now();

			std::lock_guard<std::mutex> guard(l.mutex);

			l.ofs << std::format("{:%T}", now);
			l.ofs << " - ";
			l.ofs << '[' << std::setw(5) << std::right << std::this_thread::get_id() << "] ";
			l.ofs.write(msg.c_str(), msg.size());
			l.ofs.put('\n');
			l.ofs.flush();
		}

		static void trace(const std::string &msg) {if (get().level <= log_level::TRACE) writeln("[TRACE] " + msg);}
		static void info(const std::string &msg) {if (get().level <= log_level::INFO) writeln("[INFO] " + msg);}
		static void warn(const std::string &msg) {if (get().level <= log_level::WARN) writeln("[WARN] " + msg);}
		static void error(const std::string &msg) {if (get().level <= log_level::ERROR) writeln("[ERROR] " + msg);}
		static void critical(const std::string &msg) {if (get().level <= log_level::CRITICAL) writeln("[CRITICAL] " + msg);}

	private:
		logger() = default;

		static logger &get()
		{
			static logger l;
			return l;
		}

	private:
		std::ofstream ofs;
		std::mutex mutex;
		log_level level = log_level::TRACE;
	};
}

#endif /* HILL__LSP__LOGGER_HPP_INCLUDED */
