#ifndef HILL__LSP__LISTENER_HPP_INCLUDED
#define HILL__LSP__LISTENER_HPP_INCLUDED

#include "request.hpp"
#include "server_state.hpp"
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

namespace hill::lsp {

	struct listener {
		static std::optional<std::shared_ptr<request>> get_req()
		{
			auto &state = server_state::get();

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

		static std::optional<std::unordered_map<std::string, std::string>> recv_headers()
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
						auto &state = server_state::get();
						state.log.error("Received line without ':' that is not empty [" + std::string(b) + "]");
						return std::nullopt;
					}

					return headers;
				}
			}
		}

		static std::optional<std::string> recv_content(const std::unordered_map<std::string, std::string> &headers)
		{
			if (!headers.contains("Content-Length")) {
				auto &state = server_state::get();
				state.log.error("Request missing required header [Content-Length]");
				return std::nullopt;
			}

			char *endp = nullptr;
			size_t content_len = std::stoull(headers.at("Content-Length").c_str());

			std::string content;
			while (content.size() < content_len) {
				content.push_back(getchar());
			}

			return content;
		}

		static std::optional<std::shared_ptr<utils::json_value>> parse_content(const std::string &content)
		{
			std::istringstream istr(content);
			utils::json_parser parser;
			auto json = parser.parse(istr);

			if (json.has_value()) {
				return json.value();
			} else {
				auto &state = server_state::get();
				state.log.error("Failed to parse json content");
				return std::nullopt;
			}
		}

		static std::optional<std::shared_ptr<request>> build_request(const std::shared_ptr<utils::json_value> &json)
		{
			using namespace ::hill::utils;

			if (json->kind != json_value_kind::OBJECT) return std::nullopt;
			if (!json->object_contains("method") || !json->object_contains("jsonrpc")) {
				auto &state = server_state::get();
				state.log.error("Missing required json fields in object - " + json->to_str());
				return std::nullopt;
			}
			
			auto method_json = json->object("method");
			if (!method_json.has_value()) return std::nullopt;
			if (method_json.value()->kind != json_value_kind::STRING) return std::nullopt;
			auto method = method_parse(method_json.value()->string);
			if (!method.has_value()) {
				auto &state = server_state::get();
				state.log.error("Unknown method [" + method_json.value()->string + "]");
				return std::nullopt;
			}

			if (json->object_contains("id")) {
				auto id = json->object("id");
				if (!id.has_value()) return std::nullopt;
				if (id.value()->kind != json_value_kind::NUMBER) return std::nullopt;

				if (!json->object_contains("params")) return std::nullopt;
				auto params = json->object("params");
				if (!params.has_value()) return std::nullopt;
				if (params.value()->kind != json_value_kind::OBJECT) return std::nullopt;

				return std::make_shared<request>(method.value(), (size_t)id.value()->number, params.value());
			} else {
				return std::make_shared<request>(method.value());
			}
		}
	};
}

#endif /* HILL__LSP__LISTENER_HPP_INCLUDED */
