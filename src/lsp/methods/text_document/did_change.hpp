#ifndef HILL__LSP__METHODS__TEXT_DOCUMETN__DID_CHANGE_HPP_INCLUDED
#define HILL__LSP__METHODS__TEXT_DOCUMETN__DID_CHANGE_HPP_INCLUDED

#include "../../models.hpp"
#include "../../logger.hpp"
#include "../../server_state.hpp"

#include <memory>
#include <optional>
#include <variant>
#include <string>

namespace hill::lsp::methods {

	inline void text_document_did_change(const models::notification_message &req)
	{
		auto &state = server_state::get();
		if (!state.server_capabilities.text_document_sync.has_value()) return;
		auto &text_document_sync = state.server_capabilities.text_document_sync.value();

		auto params_opt = models::did_change_text_document_params::from_json(req.params.value());
		// TODO: Handle maulformed params
		auto &params = params_opt.value();

		if (text_document_sync==models::text_document_sync_kind::FULL) {
			logger::trace("textDocument/didChange kind<Full> Uri<" + params.text_document.uri + ">");
			state.document_store.set(params.text_document.uri, params.content_changes.back().text);
		} else {
			logger::error("textDocument/didChange Unknown kind<" + std::to_string((int)text_document_sync) + ">");
		}
	}
};

#endif /* HILL__LSP__METHODS__TEXT_DOCUMETN__DID_CHANGE_HPP_INCLUDED */
