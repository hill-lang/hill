#ifndef HILL__LSP__SERVER_HPP_INCLUDED
#define HILL__LSP__SERVER_HPP_INCLUDED

// https://microsoft.github.io/language-server-protocol/specifications/lsp/3.17/specification/

#include "server_state.hpp"
#include "listener.hpp"
#include "request_handler.hpp"
#include "thread_pool.hpp"

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
		lsp::thread_pool thread_pool;

		void run()
		{
			running = true;

			auto &state = server_state::get();
			state.log.info("Hill language server starting ...");

			state.log.info("Starting thread pool ...");
			thread_pool.start();

			while (running) {
				state.log.trace("Receiving ...");
				auto req = listener::next();
				if (!req.has_value()) continue;

				auto req_ptr = req.value();
				thread_pool.queue_job([req_ptr] {request_handler::handle(req_ptr);});
			}

			state.log.info("Shutting down ...");

			state.log.info("Stoping thread pool ...");
			thread_pool.stop();
			state.log.info("Joining thread pool ...");
			thread_pool.join();

			state.log.info("Sucessfully shut down");
		}
	};
}

#endif /* HILL__LSP__SERVER_HPP_INCLUDED */
