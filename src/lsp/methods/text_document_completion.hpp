#ifndef HILL__LSP__METHODS_TEXT_DOCUMENT_COMPLETION_HPP_INCLUDED
#define HILL__LSP__METHODS_TEXT_DOCUMENT_COMPLETION_HPP_INCLUDED

#include "../models.hpp"
#include "../server_state.hpp"

#include <memory>
#include <optional>
#include <variant>

namespace hill::lsp::methods {

	// Requset to initialize
	inline std::variant<models::result_t, models::response_error> text_document_completion(const models::request_message &req)
	{
		auto &state = server_state::get();
		state.log.trace("text_document_completion() " + req.params.value()->to_str());
		throw;
	}
};

#endif /* HILL__LSP__METHODS_TEXT_DOCUMENT_COMPLETION_HPP_INCLUDED */
