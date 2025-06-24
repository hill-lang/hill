#ifndef HILL__LSP__LISTENER_HH_INCLUDED
#define HILL__LSP__LISTENER_HH_INCLUDED

#include "models.hh"
#include "logger.hh"
#include "../exceptions.hh"
#include "../utils/json_parser.hh"

#include <filesystem>
#include <fstream>
#include <stddef.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <unordered_map>
#include <memory>
#include <optional>

namespace hill::lsp {

	struct listener {
		listener() = delete;

		static std::optional<std::shared_ptr<utils::json_value>> next()
		{
			auto headers = recv_headers();
			if (!headers.has_value()) return std::nullopt;

			auto content = recv_content(headers.value());
			if (!content.has_value()) return std::nullopt;

			auto json = parse_content(content.value());
			if (!json.has_value()) return std::nullopt;

			return json.value();
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
				if (!fgets(b, MAX_HTTP_HEADER_LENGTH, stdin)) {
					continue;
				}

				p = strchr(b, '\r');
				if (p) {
					*p = '\0';
				}

				p = strchr(b, ':');
				if (p) {
					*p++ = '\0';
					while (isspace(*p)) ++p;
					headers[b] = p;
				} else {
					if (*b) {
						logger::error("Received line without ':' that is not empty [" + std::string(b) + "]");
						return std::nullopt;
					}

					return headers;
				}
			}
		}

		static std::optional<std::string> recv_content(const std::unordered_map<std::string, std::string> &headers)
		{
			if (!headers.contains("Content-Length")) {
				logger::error("Request missing required header [Content-Length]");
				return std::nullopt;
			}

			size_t content_len = std::stoull(headers.at("Content-Length").c_str());

			std::string content;
			while (content.size() < content_len) {
				content.push_back((char)getchar());
			}

			return content;
		}

		static std::optional<std::shared_ptr<utils::json_value>> parse_content(const std::string &content)
		{
			std::istringstream istr(content);
			auto json = utils::json_parser::parse(istr);

			if (json.has_value()) {
				return json.value();
			} else {
				logger::error("Failed to parse json content");
				return std::nullopt;
			}
		}
	};
}

#endif /* HILL__LSP__LISTENER_HH_INCLUDED */
