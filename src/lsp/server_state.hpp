#ifndef HILL__LSP__SERVER_STATE_HPP_INCLUDED
#define HILL__LSP__SERVER_STATE_HPP_INCLUDED

#include "document_store.hpp"

#include <atomic>

namespace hill::lsp {

	struct server_state {
		std::atomic<bool> running = true;
		std::atomic<bool> initialized = false;

		lsp::document_store document_store;

		static server_state &get()
		{
			static server_state state;
			return state;
		}
	};
}

#endif /* HILL__LSP__SERVER_STATE_HPP_INCLUDED */
