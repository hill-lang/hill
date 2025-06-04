#ifndef HILL__LSP__LISTENER_HPP_INCLUDED
#define HILL__LSP__LISTENER_HPP_INCLUDED

#include "request.hpp"
#include "../exceptions.hpp"

#include <filesystem>
#include <fstream>
#include <stddef.h>
#include <string>
#include <unordered_map>
#include <memory>

namespace hill::lsp {

	struct listener {

		std::shared_ptr<request> get_req()
		{
			reset();

			while (!recv_headers()) {}

			parse_headers();

			while (!recv_content()) {}

			parse_content();

			return std::make_shared<request>(method::INITIALIZE);
		}

	private:
		enum { MAX_HTTP_HEADER_LENGTH = 512 };

		char b[MAX_HTTP_HEADER_LENGTH];
		std::unordered_map<std::string, std::string> headers;
		size_t content_len;
		std::string content_buf;

	private:
		void reset()
		{
			content_len = 0u;
			content_buf.clear();
			headers.clear();
		}

		bool recv_headers()
		{
			char *p;

			fgets(b, MAX_HTTP_HEADER_LENGTH, stdin);

			if ((p = strchr(b, '\r'))) {
				*p = '\0';
			}

			if ((p = strchr(b, ':'))) {
				*p++ = '\0';
				while (isspace(*p)) ++p;
				headers[b] = p;

				return false;
			} else {
				if (*b) {
					throw internal_exception();
				}
				return true;
			}
		}

		bool parse_headers()
		{
			if (!headers.contains("Content-Length")) {
				throw internal_exception();
			}

			char *endp = nullptr;
			content_len = std::strtoull(headers.at("Content-Length").c_str(), &endp, 10);

			return true;
		}

		bool recv_content()
		{
			content_buf.push_back(getchar());
			return content_buf.size() >= content_len;
		}

		bool parse_content()
		{
			/*logs << content_buf << '\n';
			logs.flush();*/

			return true;
		}
	};
}

#endif /* HILL__LSP__LISTENER_HPP_INCLUDED */
