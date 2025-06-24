#ifndef HILL__LSP__METHODS_TEXT_DOCUMENT__FORMATTING_HH_INCLUDED
#define HILL__LSP__METHODS_TEXT_DOCUMENT__FORMATTING_HH_INCLUDED

#include "../../models.hh"
#include "../../logger.hh"
#include "../../server_state.hh"

#include "../../../fmt/formatter.hh"

namespace hill::lsp::methods {

	inline std::variant<std::optional<models::result_t>, models::response_error> text_document_formatting(const models::request_message &req)
	{
		auto params = models::document_formatting_params::from_json(req.params.value()).value();
		(void)params; // Unused

		//auto output = fmt::formatter::format();

		std::vector<models::text_edit> text_edits = {
			{
				.range = models::range{
					.start = models::position{
						.line = 0u,
						.character = 0u},
					.end = models::position{
						.line = 0u,
						.character = 0u},
				},
				.new_text = "Imagine if this actually worked\n\n"
			}
		};

		auto json = utils::json_value::create<utils::json_value_kind::ARRAY>();
		for (const auto &edit: text_edits) {json->arr_add_obj(edit.json());}
		return json;
	}
}

#endif /* HILL__LSP__METHODS_TEXT_DOCUMENT__FORMATTING_HH_INCLUDED */
