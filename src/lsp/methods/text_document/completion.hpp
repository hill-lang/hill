#ifndef HILL__LSP__METHODS_TEXT_DOCUMENT__COMPLETION_HPP_INCLUDED
#define HILL__LSP__METHODS_TEXT_DOCUMENT__COMPLETION_HPP_INCLUDED

#include "../../models.hpp"
#include "../../logger.hpp"

#include <memory>
#include <optional>
#include <variant>

namespace hill::lsp::methods {

	// Requset to initialize
	inline std::variant<models::result_t, models::response_error> text_document_completion(const models::request_message &req)
	{
		auto params = models::text_document_position_params::from_json(req.params.value());

		models::completion_list completion_list = {
			.is_incomplete = false,
			.items = {
				models::completion_item {.label="@i8"},
				models::completion_item {.label="@i16"},
				models::completion_item {.label="@i32"},
				models::completion_item {.label="@i64"},
				models::completion_item {.label="@i128"},

				models::completion_item {.label="i8"},
				models::completion_item {.label="i16"},
				models::completion_item {.label="i32"},
				models::completion_item {.label="i64"},
				models::completion_item {.label="i128"},

				models::completion_item {.label="@u8"},
				models::completion_item {.label="@u16"},
				models::completion_item {.label="@u32"},
				models::completion_item {.label="@u64"},
				models::completion_item {.label="@u128"},

				models::completion_item {.label="u8"},
				models::completion_item {.label="u16"},
				models::completion_item {.label="u32"},
				models::completion_item {.label="u64"},
				models::completion_item {.label="u128"},

				models::completion_item {.label="@f8"},
				models::completion_item {.label="@f16"},
				models::completion_item {.label="@f32"},
				models::completion_item {.label="@f64"},

				models::completion_item {.label="f8"},
				models::completion_item {.label="f16"},
				models::completion_item {.label="f32"},
				models::completion_item {.label="f64"},

				models::completion_item {.label="mut"},
				models::completion_item {.label="@mut"},
			}
		};

		return completion_list.json();
	}
};

#endif /* HILL__LSP__METHODS_TEXT_DOCUMENT__COMPLETION_HPP_INCLUDED */
