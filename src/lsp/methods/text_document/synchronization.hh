#ifndef HILL__LSP__METHODS__TEXT_DOCUMETN__SYNCHRONIZATION_HH_INCLUDED
#define HILL__LSP__METHODS__TEXT_DOCUMETN__SYNCHRONIZATION_HH_INCLUDED

#include "../../models.hh"
#include "../../logger.hh"
#include "../../server_state.hh"

#include <string>

namespace hill::lsp::methods {

	inline void text_document_did_open(const models::notification_message &req)
	{
		auto params = *models::did_open_text_document_params::from_json(*req.params);

		logger::trace("textDocument/didOpen Uri<" + params.text_document.uri
			+ "> langId<" + params.text_document.language_id + ">");
	}

	inline void text_document_did_change(const models::notification_message &req)
	{
		auto &state = server_state::get();
		if (!state.server_capabilities.text_document_sync) return;
		auto &text_document_sync = *state.server_capabilities.text_document_sync;

		auto params = *models::did_change_text_document_params::from_json(*req.params);

		if (text_document_sync == models::text_document_sync_kind::FULL) {
			logger::trace("textDocument/didChange kind<Full> uri<" + params.text_document.uri + ">");
			state.document_store.set(params.text_document.uri, params.content_changes.back().text);
		} else {
			logger::error("textDocument/didChange Unknown kind<" + std::to_string((int)text_document_sync) + ">");
		}
	}

	// Will save
	// Will save wait untill
	// Did save

	inline void text_document_did_close(const models::notification_message &req)
	{
		auto &state = server_state::get();

		auto params = *models::did_close_text_document_params::from_json(*req.params);

		logger::trace("textDocument/didClose uri<" + params.text_document.uri + ">");

		state.document_store.remove(params.text_document.uri);
	}

	// Rename
};

#endif /* HILL__LSP__METHODS__TEXT_DOCUMETN__SYNCHRONIZATION_HH_INCLUDED */
