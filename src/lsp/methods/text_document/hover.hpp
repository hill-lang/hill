#ifndef HILL__LSP__METHODS_TEXT_DOCUMENT__HOVER_HPP_INCLUDED
#define HILL__LSP__METHODS_TEXT_DOCUMENT__HOVER_HPP_INCLUDED

#include "../../models.hpp"
#include "../../logger.hpp"
#include "../../server_state.hpp"

namespace hill::lsp::methods {

	inline std::variant<std::optional<models::result_t>, models::response_error> text_document_hover(const models::request_message &req)
	{
		return std::nullopt;
	}
}

#endif /* HILL__LSP__METHODS_TEXT_DOCUMENT__HOVER_HPP_INCLUDED */
