#ifndef HILL__LSP__SERVER_HPP_INCLUDED
#define HILL__LSP__SERVER_HPP_INCLUDED

// https://microsoft.github.io/language-server-protocol/specifications/lsp/3.17/specification/

#include "server_state.hpp"
#include "listener.hpp"
#include "router.hpp"
#include "response_builder.hpp"

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

			auto &state = server_state::get();
			state.log.open("./tmp/hill-lsp.log");
		}

		bool running;
		lsp::listener listener;
		lsp::router router;
		lsp::response_builder res_builder;

		void run()
		{
			running = true;

			auto &state = server_state::get();
			state.log.info("Hill language server started");

			while (running) {
				state.log.trace("Receiving ...");
				auto req = listener.get_req();
				if (!req.has_value()) continue;

				auto metstr = std::string(method_str(req.value()->metod));

				if (req.value()->is_notification()) {
					state.log.info("Received notification method [" + metstr + "]");
				} else {
					state.log.info("Received request method [" + metstr + "] id: [" + std::to_string(req.value()->id.value()) + "] ");
				}

				auto func = router.get(req.value()->metod);
				if (!func.has_value()) {
					state.log.error("Fail to resolve method " + metstr);
					continue;
				}

				auto result = func.value()(req.value()->params);
				if (!result.has_value()) continue;

				// We only have to reply to messages with an id
				if (req.value()->is_notification()) continue;

				//auto resp = res_builder.build(req->id, result);
				auto resp = handle_request(req.value()->id.value());
				state.log.trace(resp);

				fwrite(resp.c_str(), 1, resp.size(), stdout);
				fflush(stdout);
			}
		}

	private:
		std::string handle_request(size_t id)
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

			return res_builder.build(id, result);
		}
	};
}

#endif /* HILL__LSP__SERVER_HPP_INCLUDED */
