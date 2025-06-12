#ifndef HILL__LSP__METHODS_TEXT_DOCUMENT__FORMATTING_HPP_INCLUDED
#define HILL__LSP__METHODS_TEXT_DOCUMENT__FORMATTING_HPP_INCLUDED

#include "../../models.hpp"
#include "../../logger.hpp"
#include "../../server_state.hpp"

#include "../../../fmt/formatter.hpp"

namespace hill::lsp::methods {

	inline std::variant<std::optional<models::result_t>, models::response_error> text_document_formatting(const models::request_message &req)
	{
		auto params = models::document_formatting_params::from_json(req.params.value()).value();
		(void)params; // Unused

		//auto output = fmt::formatter::format();

		std::vector<models::text_edit> text_edits = {
			{
				.range = {.start=0, .end=0},
				.new_text = "Imagine if this actually worked\n\n"
			}
		};

		auto json = utils::json_value::create<utils::json_value_kind::ARRAY>();
		for (const auto &edit: text_edits) {json->arr_add_obj(edit.json());}
		return json;
	}
}

#endif /* HILL__LSP__METHODS_TEXT_DOCUMENT__FORMATTING_HPP_INCLUDED */
