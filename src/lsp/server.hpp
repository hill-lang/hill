#ifndef HILL__LSP__SERVER_HPP_INCLUDED
#define HILL__LSP__SERVER_HPP_INCLUDED

// https://microsoft.github.io/language-server-protocol/specifications/lsp/3.17/specification/

#include "logger.hpp"
#include "listener.hpp"
#include "router.hpp"
#include "response_builder.hpp"

#include <string>
#include <stddef.h>
#include <stdint.h>

// TODO: Move to response builder?
#include "../utils/json_writer.hpp"

namespace hill::lsp {

	struct server {
		server() : running(true)
		{
			auto &state = get_state();
			state.log.open("./tmp/hill-lsp.log");
		}

		bool running;
		lsp::listener listener;
		lsp::router router;
		lsp::response_builder res_builder;

		void run()
		{
			running = true;

			auto &state = get_state();
			state.log.writeln("Server start");

			while (running) {
				auto req = listener.get_req();
				if (!req.has_value()) continue;

				auto func = router.get(req.value()->metod);
				if (!func.has_value()) continue;

				//auto result = func(req->content);
				auto result = func.value()();
				if (!result) continue;

				// We only have to reply to messages with an id
				if (!req.value()->id.has_value()) continue;

				//auto response = res_builder.build(req->id, result);
				auto response = handle_request();

				fwrite(response.c_str(), 1, response.size(), stdout);
				fflush(stdout);
			}
		}

		struct server_state {
			logger log;
		};

		static server_state &get_state()
		{
			static server_state log;
			return log;
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
