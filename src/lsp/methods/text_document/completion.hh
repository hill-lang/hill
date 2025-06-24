#ifndef HILL__LSP__METHODS_TEXT_DOCUMENT__COMPLETION_HH_INCLUDED
#define HILL__LSP__METHODS_TEXT_DOCUMENT__COMPLETION_HH_INCLUDED

#include "../../models.hh"
#include "../../logger.hh"
#include "../../server_state.hh"

namespace hill::lsp::methods {

	// Requset to initialize
	inline std::variant<std::optional<models::result_t>, models::response_error> text_document_completion(const models::request_message &req)
	{
		(void)req; // Unused
		//auto &state = server_state::get();
		//auto params = models::text_document_position_params::from_json(req.params.value()).value();

		// TODO: We kinda need the document
		//state.document_store.exists(params.text_document.uri);

		//params.position.line;
		//params.position.character;

		//auto content = state.document_store.get(params.text_document.uri).value();
		//logger::trace(content);

		 auto result = std::vector<models::completion_item> {
			{.label="@i8", .kind=models::completion_item_kind::FUNCTION, .detail="8-bit signed integer"},
			{.label="@i16", .kind=models::completion_item_kind::FUNCTION, .detail="16-bit signed integer"},
			{.label="@i32", .kind=models::completion_item_kind::FUNCTION, .detail="32-bit signed integer"},
			{.label="@i64", .kind=models::completion_item_kind::FUNCTION, .detail="64-bit signed integer"},
			{.label="@i128", .kind=models::completion_item_kind::FUNCTION, .detail="128-bit signed integer"},

			{.label="i8", .kind=models::completion_item_kind::FUNCTION, .detail="8-bit integer"},
			{.label="i16", .kind=models::completion_item_kind::FUNCTION, .detail="16-bit integer"},
			{.label="i32", .kind=models::completion_item_kind::FUNCTION, .detail="32-bit integer"},
			{.label="i64", .kind=models::completion_item_kind::FUNCTION, .detail="64-bit integer"},
			{.label="i128", .kind=models::completion_item_kind::FUNCTION, .detail="128-bit integer"},

			{.label="@u8", .kind=models::completion_item_kind::FUNCTION, .detail="8-bit unsigned integer"},
			{.label="@u16", .kind=models::completion_item_kind::FUNCTION, .detail="16-bit unsigned integer"},
			{.label="@u32", .kind=models::completion_item_kind::FUNCTION, .detail="32-bit unsigned integer"},
			{.label="@u64", .kind=models::completion_item_kind::FUNCTION, .detail="64-bit unsigned integer"},
			{.label="@u128", .kind=models::completion_item_kind::FUNCTION, .detail="128-bit unsigned integer"},

			{.label="u8", .kind=models::completion_item_kind::FUNCTION, .detail="8-bit unsigned integer"},
			{.label="u16", .kind=models::completion_item_kind::FUNCTION, .detail="16-bit unsigned integer"},
			{.label="u32", .kind=models::completion_item_kind::FUNCTION, .detail="32-bit unsigned integer"},
			{.label="u64", .kind=models::completion_item_kind::FUNCTION, .detail="64-bit unsigned integer"},
			{.label="u128", .kind=models::completion_item_kind::FUNCTION, .detail="128-bit unsigned integer"},

			{.label="@f32", .kind=models::completion_item_kind::FUNCTION, .detail="32-bit floating point (single)"},
			{.label="@f64", .kind=models::completion_item_kind::FUNCTION, .detail="64-bit floating point (double)"},

			{.label="f32", .kind=models::completion_item_kind::FUNCTION, .detail="32-bit floating point (single)"},
			{.label="f64", .kind=models::completion_item_kind::FUNCTION, .detail="64-bit floating point (double)"},

			{.label="mut", .kind=models::completion_item_kind::FUNCTION, .detail="Mutable modifier"},
			{.label="@mut", .kind=models::completion_item_kind::FUNCTION, .detail="Mutable modifier"},
		};

		/*auto example = std::vector<models::completion_item> {
			{.label = "TEXT", .kind = models::completion_item_kind::TEXT},
			{
				.label = "TEXT deprecated",
				.kind = models::completion_item_kind::TEXT,
				.tags = {{models::completion_item_tag::DEPRECATED}}},
			{
				.label = "TEXT w details",
				.kind = models::completion_item_kind::TEXT,
				.detail = "Details go here"},
			{
				.label = "TEXT label details",
				.label_details = models::completion_item_label_details {
					.detail = "Details",
					.description = "Description"},
				.kind = models::completion_item_kind::TEXT},
			{.label = "METHOD", .kind = models::completion_item_kind::METHOD},
			{.label = "FUNCTION", .kind = models::completion_item_kind::FUNCTION},
			{.label = "CONSTRUCTOR", .kind = models::completion_item_kind::CONSTRUCTOR},
			{.label = "FIELD", .kind = models::completion_item_kind::FIELD},
			{.label = "VARIABLE", .kind = models::completion_item_kind::VARIABLE},
			{.label = "CLASS", .kind = models::completion_item_kind::CLASS},
			{.label = "INTERFACE", .kind = models::completion_item_kind::INTERFACE},
			{.label = "MODULE", .kind = models::completion_item_kind::MODULE},
			{.label = "PROPERTY", .kind = models::completion_item_kind::PROPERTY},
			{.label = "UNIT", .kind = models::completion_item_kind::UNIT},
			{.label = "VALUE", .kind = models::completion_item_kind::VALUE},
			{.label = "ENUM", .kind = models::completion_item_kind::ENUM},
			{.label = "KEYWORD", .kind = models::completion_item_kind::KEYWORD},
			{.label = "SNIPPET", .kind = models::completion_item_kind::SNIPPET},
			{.label = "COLOR", .kind = models::completion_item_kind::COLOR},
			{.label = "FILE", .kind = models::completion_item_kind::FILE},
			{.label = "REFERENCE", .kind = models::completion_item_kind::REFERENCE},
			{.label = "FOLDER", .kind = models::completion_item_kind::FOLDER},
			{.label = "ENUMMEMBER", .kind = models::completion_item_kind::ENUMMEMBER},
			{.label = "CONSTANT", .kind = models::completion_item_kind::CONSTANT},
			{.label = "STRUCT", .kind = models::completion_item_kind::STRUCT},
			{.label = "EVENT", .kind = models::completion_item_kind::EVENT},
			{.label = "OPERATOR", .kind = models::completion_item_kind::OPERATOR},
			{.label = "TYPEPARAMETER", .kind = models::completion_item_kind::TYPEPARAMETER}
		};*/

		models::completion_list completion_list = {
			.is_incomplete = false,
			//.items = example,
			.items = result,
		};

		return completion_list.json();
	}
};

#endif /* HILL__LSP__METHODS_TEXT_DOCUMENT__COMPLETION_HH_INCLUDED */
