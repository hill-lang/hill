#ifndef HILL__LSP__SERVER_STATE_HPP_INCLUDED
#define HILL__LSP__SERVER_STATE_HPP_INCLUDED

#include "logger.hpp"
#include "document_store.hpp"
#include <atomic>

namespace hill::lsp {

	struct server_state {
		logger log;

		std::atomic_bool initialized = false;
		lsp::document_store document_store;

		static server_state &get()
		{
			static server_state log;
			return log;
		}
	};
}

#endif /* HILL__LSP__SERVER_STATE_HPP_INCLUDED */
