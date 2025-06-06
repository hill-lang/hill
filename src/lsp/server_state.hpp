#ifndef HILL__LSP__SERVER_STATE_HPP_INCLUDED
#define HILL__LSP__SERVER_STATE_HPP_INCLUDED

#include "document_store.hpp"
#include "models.hpp"

#include <atomic>

namespace hill::lsp {

	struct server_state {
		std::atomic<bool> running = true;
		std::atomic<bool> initialized = false;

		lsp::document_store document_store;

		const models::server_capabilities server_capabilities = {
			.text_document_sync = models::text_document_sync_kind::FULL,
			.completion_provider = models::completion_options{},
		};

		static server_state &get()
		{
			static server_state state;
			return state;
		}
	};
}

#endif /* HILL__LSP__SERVER_STATE_HPP_INCLUDED */
