#ifndef HILL__LSP__LISTENER_HPP_INCLUDED
#define HILL__LSP__LISTENER_HPP_INCLUDED

#include "request.hpp"
#include "server.hpp"
#include "../exceptions.hpp"
#include "../utils/json_parser.hpp"

#include <filesystem>
#include <fstream>
#include <stddef.h>
#include <string>
#include <sstream>
#include <unordered_map>
#include <memory>
#include <optional>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

namespace hill::lsp {

	struct listener {
		listener()
		{
#ifdef _WIN32
			(void)_setmode(_fileno(stdin), _O_BINARY);
			(void)_setmode(_fileno(stdout), _O_BINARY);
#endif
		}

		std::optional<std::shared_ptr<request>> get_req()
		{
			auto headers = recv_headers();
			if (!headers.has_value()) return std::nullopt;

			auto content = recv_content(headers.value());
			if (!content.has_value()) return std::nullopt;

			auto json = parse_content(content.value());
			if (!json.has_value()) return std::nullopt;

			return build_request(json.value());
		}

	private:
		enum {MAX_HTTP_HEADER_LENGTH=512};

		std::optional<std::unordered_map<std::string, std::string>> recv_headers()
		{
			std::unordered_map<std::string, std::string> headers;
			char b[MAX_HTTP_HEADER_LENGTH];

			// TODO: Handle timeouts?

			while (true) {
				char *p = NULL;
				fgets(b, MAX_HTTP_HEADER_LENGTH, stdin);

				if ((p = strchr(b, '\r'))) {
					*p = '\0';
				}

				if ((p = strchr(b, ':'))) {
					*p++ = '\0';
					while (isspace(*p)) ++p;
					headers[b] = p;
				} else {
					if (*b) {
						//auto &state = server::get_state();
						//state.log.error("Received line without ':' that is not empty");
						return std::nullopt;
					}
					return headers;
				}
			}
		}

		std::optional<std::string> recv_content(const std::unordered_map<std::string, std::string> &headers)
		{
			if (!headers.contains("Content-Length")) {
				//auto &state = server::get_state();
				//state.log.error("Request missing required header [Content-Length]");
				return std::nullopt;
			}

			char *endp = nullptr;
			size_t content_len = (size_t)std::strtoull(headers.at("Content-Length").c_str(), &endp, 10);

			std::string content;
			while (content.size() < content_len) {
				content.push_back(getchar());
			}

			return content;
		}

		std::optional<std::shared_ptr<utils::json_value>> parse_content(const std::string &content)
		{
			std::istringstream istr(content);
			utils::json_parser parser;
			auto json = parser.parse(istr);

			if (json.has_value()) {
				return json.value();
			} else {
				//auto &state = server::get_state();
				//state.log.error("Failed to parse json content");
				return std::nullopt;
			}
		}

		std::optional<std::shared_ptr<request>> build_request(const std::shared_ptr<utils::json_value> &json)
		{
			using namespace ::hill::utils;

			if (json->kind != json_value_kind::OBJECT) return std::nullopt;
			if (!json->object_contains("jsonrpc")) return std::nullopt;
			if (!json->object_contains("method")) return std::nullopt;
			if (!json->object_contains("params")) return std::nullopt;

			auto method_json = json->object("method");
			if (!method_json.has_value()) return std::nullopt;
			if (method_json.value()->kind != json_value_kind::STRING) return std::nullopt;
			auto method = method_parse(method_json.value()->string);
			if (!method.has_value()) return std::nullopt;

			auto params = json->object("params");
			if (!params.has_value()) return std::nullopt;
			if (params.value()->kind != json_value_kind::OBJECT) return std::nullopt;

			if (json->object_contains("id")) {
				auto id = json->object("id");
				if (!id.has_value()) return std::nullopt;
				if (id.value()->kind != json_value_kind::NUMBER) return std::nullopt;

				return std::make_shared<request>(method.value(), (size_t)id.value()->number, params.value());
			} else {
				return std::make_shared<request>(method.value(), params.value());
			}
		}
	};
}

#endif /* HILL__LSP__LISTENER_HPP_INCLUDED */
