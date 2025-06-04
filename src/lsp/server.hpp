#ifndef HILL__LSP__SERVER_HPP_INCLUDED
#define HILL__LSP__SERVER_HPP_INCLUDED

// https://microsoft.github.io/language-server-protocol/specifications/lsp/3.17/specification/

#include "logger.hpp"
#include "listener.hpp"
#include "router.hpp"
#include "response_builder.hpp"
#include "../exceptions.hpp"
#include "../utils/json_writer.hpp"

#include <string>
#include <stddef.h>
#include <stdint.h>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

namespace hill::lsp {

	struct server {
		server() : running(true)
		{
#ifdef _WIN32
			(void)_setmode(_fileno(stdin), _O_BINARY);
			(void)_setmode(_fileno(stdout), _O_BINARY);
#endif
		}

		bool running;
		logger log;
		listener listener;
		router router;
		response_builder res_builder;

		void run()
		{
			running = true;
			log.open("/tmp/lsp.log");

			while (running) {
				auto req = listener.get_req();
				if (!req) continue;

				auto func = router.get(req->metod);
				if (!func) continue;

				//auto result = func(req->content);
				auto result = func();
				if (!result) continue;

				// We only have to reply to messages with an id
				if (req->has_id) continue;

				//auto response = res_builder.build(req->id, result);
				auto response = handle_request();

				fwrite(response.c_str(), 1, response.size(), stdout);
				fflush(stdout);
			}
		}

	private:
		std::string handle_request()
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

			return res_builder.build(0, result);
		}
	};
}

#endif /* HILL__LSP__SERVER_HPP_INCLUDED */
