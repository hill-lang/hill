#ifndef HILL__LSP__SERVER_HPP_INCLUDED
#define HILL__LSP__SERVER_HPP_INCLUDED

// https://microsoft.github.io/language-server-protocol/specifications/lsp/3.17/specification/

#include "../exceptions.hpp"
#include "../utils/json_writer.hpp"

#include <iostream>
#include <string>
#include <string_view>
#include <stddef.h>
#include <stdint.h>
#include <unordered_map>

#include <filesystem>
#include <fstream>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

namespace hill::lsp {

	enum class srv_state {
		ERROR,
		RECV_HEADERS,
		PARSE_HEADERS,
		RECV_CONTENT,
		PARSE_CONTENT,
		HANDLE_REQUEST,
	};

	struct server {
		server() = default;

		// DEBUG: For logging
		std::ofstream os;

		void run()
		{
#ifdef _WIN32
			_setmode(_fileno(stdout), _O_BINARY);
			_setmode(_fileno(stdin), _O_BINARY);
#endif

			std::filesystem::create_directories("/tmp/");
			os.open("/tmp/lsp.log", std::ios::binary);

			running = true;
			header_buf.reserve(256u);
			content_buf.reserve(16'384u);
			reset();

			while (running) {
				switch (state) {
				case srv_state::ERROR: reset(); break;
				case srv_state::RECV_HEADERS: if (recv_headers()) {state=srv_state::PARSE_HEADERS;} break;
				case srv_state::PARSE_HEADERS: if (parse_headers()) {state=srv_state::RECV_CONTENT;} break;
				case srv_state::RECV_CONTENT: if (recv_content()) {state=srv_state::PARSE_CONTENT;} break;
				case srv_state::PARSE_CONTENT: if (parse_content()) {state=srv_state::HANDLE_REQUEST;} break;
				case srv_state::HANDLE_REQUEST: handle_request(); reset(); break;
				default: throw internal_exception();
				}
			}
		}

	private:
		bool running;

		srv_state state;

		size_t content_len;
		std::string header_buf;
		std::string content_buf;

		std::unordered_map<std::string, std::string> headers;

	private:
		void reset()
		{
			state = srv_state::RECV_HEADERS;
			header_buf.clear();
			content_buf.clear();
			content_len = 0u;
			headers.clear();
		}

		bool recv_headers()
		{
			char c = std::cin.get();
			if (c == '\r') return false;
			header_buf.push_back(c);

			// Keep reading headers untill the next new line
			if (header_buf.back() != '\n') {
				return false;
			}

			if (header_buf.length() > 1 || header_buf.back() != '\n') {
				size_t split_ix = header_buf.find_first_of(':');
				headers[header_buf.substr(0, split_ix)] = header_buf.substr(split_ix+2, header_buf.length()-split_ix-3);
				header_buf.clear();
				return false;
			}

			// Two new lines in a row means that we are done reading headers
			state = srv_state::PARSE_HEADERS;
			return true;
		}

		bool parse_headers()
		{
			if (!headers.contains("Content-Length")) {
				state = srv_state::ERROR;
				return false;
			}

			char *endp = nullptr;
			content_len = std::strtoull(headers.at("Content-Length").c_str(), &endp, 10);

			return true;
		}

		bool recv_content()
		{
			content_buf.push_back(std::cin.get());
			return content_buf.length() >= content_len;
		}

		bool parse_content()
		{
			os << content_buf << '\n';
			os.flush();

			return true;
		}

		std::string build_response(size_t id, const std::string &result)
		{
			auto oss = std::make_shared<std::ostringstream>();
			utils::json_writer json(oss);
			json.obj_i32("id", (int32_t)id);
			json.obj_raw("result", result);
			json.close();
			auto json_str = oss->str();

			std::string header = "Content-Length: " + std::to_string(json_str.size()) + "\r\n\r\n";

			// Logging
			//os.write(header.c_str(), header.size());
			os.write(json_str.c_str(), json_str.size());
			os << "\n";
			os.flush();

			std::stringstream ss;
			ss.write(header.c_str(), header.size());
			ss.write(json_str.c_str(), json_str.size());
			return ss.str();
		}

		void handle_request()
		{
			auto oss = std::make_shared<std::ostringstream>();
			utils::json_writer json(oss);
			json.obj("capabilities");
			json.obj("completionProvider");
			json.pop();
			json.pop();
			json.obj("serverInfo");
			json.obj_str("name", "hill-lsp");
			json.obj_str("version", "0.0.1");
			json.close();
			auto result = oss->str();

			auto res = build_response(0, result);

			std::cout.write(res.c_str(), res.size());
			std::cout.flush();
		}
	};
}

#endif /* HILL__LSP__SERVER_HPP_INCLUDED */
