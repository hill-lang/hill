#ifndef HILL__LSP__SERVER_HH_INCLUDED
#define HILL__LSP__SERVER_HH_INCLUDED

// https://microsoft.github.io/language-server-protocol/specifications/lsp/3.17/specification/

#include "server_state.hh"
#include "listener.hh"
#include "logger.hh"
#include "request_handler.hh"
#include "../utils/thread_pool.hh"

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

namespace hill::lsp {

	struct server {
		server()
		{
#ifdef _WIN32
			(void)_setmode(_fileno(stdin), _O_BINARY);
			(void)_setmode(_fileno(stdout), _O_BINARY);
#endif
			logger::open("./tmp/hill-lsp.log");
		}

		void run()
		{
			auto &state = server_state::get();
			state.running = true;

			logger::info("Hill language server starting ...");

			logger::info("Starting thread pool ...");
			utils::thread_pool thread_pool;
			thread_pool.start();

			while (state.running) {
				logger::trace("Receiving ...");
				auto req = listener::next();
				if (!req.has_value()) continue;

				auto req_ptr = req.value();
				thread_pool.queue_job([req_ptr] {request_handler::handle(req_ptr);});
			}

			logger::info("Shutting down ...");

			logger::info("Stopping thread pool ...");
			thread_pool.stop();
			logger::info("Joining thread pool ...");
			thread_pool.join();

			logger::info("Successfully shut down");
		}
	};
}

#endif /* HILL__LSP__SERVER_HH_INCLUDED */
