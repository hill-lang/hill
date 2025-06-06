#ifndef HILL__LSP__MODELS_HPP_INCLUDED
#define HILL__LSP__MODELS_HPP_INCLUDED

#include "../exceptions.hpp"
#include "../utils/json.hpp"

#include <optional>
#include <memory>
#include <string>
#include <vector>
#include <stdint.h>

namespace hill::lsp::models {

	enum class method {
		INITIALIZE, // Requset to initialize
		INITIALIZED, // Awknownlage initialize success
		TEXT_DOCUMENT_COMPLETION,
		TEXT_DOCUMENT_DID_CHANGE,
	};

	constexpr const char *method_str(method m)
	{
		switch (m) {
		case method::INITIALIZE: return "initialize";
		case method::INITIALIZED: return "initialized";
		case method::TEXT_DOCUMENT_COMPLETION: return "textDocument/completion";
		case method::TEXT_DOCUMENT_DID_CHANGE: return "textDocument/didChange";
		default: throw internal_exception();
		}
	}

	constexpr std::optional<method> method_parse(const std::string &str)
	{
		if (str==method_str(method::INITIALIZE)) return method::INITIALIZE;
		else if (str==method_str(method::INITIALIZED)) return method::INITIALIZED;
		else if (str==method_str(method::TEXT_DOCUMENT_COMPLETION)) return method::TEXT_DOCUMENT_COMPLETION;
		else if (str==method_str(method::TEXT_DOCUMENT_DID_CHANGE)) return method::TEXT_DOCUMENT_DID_CHANGE;
		else return std::nullopt;
	}

	enum class error_code: int {
		// Defined by JSON-RPC
		PARSE_ERROR = -32700,
		INVALID_REQUEST = -32600,
		METHOD_NOT_FOUND = -32601,
		INVALID_PARAMS = -32602,
		INTERNAL_ERROR = -32603,

		/**
		 * This is the start range of JSON-RPC reserved error codes.
		 * It doesn't denote a real error code. No LSP error codes should
		 * be defined between the start and end range. For backwards
		 * compatibility the `ServerNotInitialized` and the `UnknownErrorCode`
		 * are left in the range.
		 *
		 * @since 3.16.0
		 */
		JSONRPC_RESERVED_ERROR_RANGE_START = -32099,
		/** @deprecated use jsonrpcReservedErrorRangeStart */
		SERVER_ERROR_START = JSONRPC_RESERVED_ERROR_RANGE_START,

		/**
		 * Error code indicating that a server received a notification or
		 * request before the server received the `initialize` request.
		 */
		SERVER_NOT_INITIALIZED = -32002,
		UNKNOWN_ERROR_CODE = -32001,

		/**
		 * This is the end range of JSON-RPC reserved error codes.
		 * It doesn't denote a real error code.
		 *
		 * @since 3.16.0
		 */
		JSONRPC_RESERVED_ERROR_RANGE_END = -32000,
		/** @deprecated use jsonrpcReservedErrorRangeEnd */
		SERVER_ERROR_END = JSONRPC_RESERVED_ERROR_RANGE_END,

		/**
		 * This is the start range of LSP reserved error codes.
		 * It doesn't denote a real error code.
		 *
		 * @since 3.16.0
		 */
		LSP_RESERVED_ERROR_RANGE_START = -32899,

		/**
		 * A request failed but it was syntactically correct, e.g the
		 * method name was known and the parameters were valid. The error
		 * message should contain human readable information about why
		 * the request failed.
		 *
		 * @since 3.17.0
		 */
		REQUEST_FAILED = -32803,

		/**
		 * The server cancelled the request. This error code should
		 * only be used for requests that explicitly support being
		 * server cancellable.
		 *
		 * @since 3.17.0
		 */
		SERVER_CANCELLED = -32802,

		/**
		 * The server detected that the content of a document got
		 * modified outside normal conditions. A server should
		 * NOT send this error code if it detects a content change
		 * in its unprocessed messages. The result even computed
		 * on an older state might still be useful for the client.
		 *
		 * If a client decides that a result is not of any use anymore
		 * the client should cancel the request.
		 */
		CONTENT_MODIFIED = -32801,

		/**
		 * The client has canceled a request and a server has detected
		 * the cancel.
		 */
		REQUEST_CANCELLED = -32800,

		/**
		 * This is the end range of LSP reserved error codes.
		 * It doesn't denote a real error code.
		 *
		 * @since 3.16.0
		 */
		LSP_RESERVED_ERROR_RANGE_END = -32800,
	};

	struct request_message {
		int id;
		models::method method;
		std::optional<std::shared_ptr<::hill::utils::json_value>> params;
	};

	struct notification_message {
		models::method method;
		std::optional<std::shared_ptr<::hill::utils::json_value>> params;
	};

	struct response_error {
		error_code code;
		std::string message;
		std::optional<std::shared_ptr<::hill::utils::json_value>> data;

		std::shared_ptr<utils::json_value> json() const
		{
			using namespace ::hill::utils;
			auto json = json_value::create<json_value_kind::OBJECT>();

			json->obj_add_num("code", (double)code);
			json->obj_add_str("message", message);

			if (data.has_value()) {
				json->obj_add_obj("data", data.value());
			}

			return json;
		}
	};

	typedef std::shared_ptr<::hill::utils::json_value> result_t;

	struct response_message {
		int id;
		std::optional<result_t> result;
		std::optional<response_error> error;

		std::shared_ptr<utils::json_value> json() const
		{
			auto json = utils::json_value::create<utils::json_value_kind::OBJECT>();

			json->obj_add_num("id", (double)id);

			if (result.has_value()) {
				json->obj_add_obj("result", result.value());
			}

			if (error.has_value()) {
				json->obj_add_obj("error", error.value().json());
			}

			return json;
		}
	};

	enum class text_document_sync_kind : int {
		/**
		 * Documents should not be synced at all.
		 */
		NONE = 0,

		/**
		 * Documents are synced by always sending the full content
		 * of the document.
		 */
		FULL = 1,

		/**
		 * Documents are synced by sending the full content on open.
		 * After that only incremental updates to the document are
		 * sent.
		 */
		INCREMENTAL = 2,
	};

	struct completion_options {

		std::shared_ptr<utils::json_value> json() const
		{
			auto json = utils::json_value::create<utils::json_value_kind::OBJECT>();

			return json;
		}
	};

	struct server_capabilities {

		std::optional<text_document_sync_kind> text_document_sync;
		std::optional<completion_options> completion_provider;

		std::shared_ptr<utils::json_value> json() const
		{
			auto json = utils::json_value::create<utils::json_value_kind::OBJECT>();

			if (text_document_sync.has_value()) {
				json->obj_add_num("textDocumentSync", (int)text_document_sync.value());
			}

			if (completion_provider.has_value()) {
				json->obj_add_obj("completionProvider", completion_provider.value().json());
			}
			
			return json;
		}
	};

	struct server_info {
		std::string name;
		std::optional<std::string> version;

		std::shared_ptr<utils::json_value> json() const
		{
			auto json = utils::json_value::create<utils::json_value_kind::OBJECT>();

			json->obj_add_str("name", name);

			if (version.has_value()) {
				json->obj_add_str("version", version.value());
			}

			return json;
		}
	};

	struct initialize_result {
		server_capabilities capabilities;
		std::optional<models::server_info> server_info;

		std::shared_ptr<utils::json_value> json() const
		{
			auto json = utils::json_value::create<utils::json_value_kind::OBJECT>();

			json->obj_add_obj("capabilities", capabilities.json());

			if (server_info.has_value()) {
				json->obj_add_obj("serverInfo", server_info.value().json());
			}

			return json;
		}
	};

	struct position {
		uint32_t line;
		uint32_t character;

		static std::optional<position> from_json(const std::shared_ptr<utils::json_value> &json)
		{
			using namespace ::hill::utils;

			if (json->kind()!=json_value_kind::OBJECT) return std::nullopt;
			if (!json->obj_has("line")) return std::nullopt;
			if (!json->obj_has("character")) return std::nullopt;

			auto ln_json = json->obj_get("line").value();
			auto ch_json = json->obj_get("character").value();

			if (ln_json->kind()!=json_value_kind::NUMBER) return std::nullopt;
			if (ch_json->kind()!=json_value_kind::NUMBER) return std::nullopt;

			return position{
				.line = (uint32_t)ln_json->num().value(),
				.character = (uint32_t)ch_json->num().value(),
			};
		}

		std::shared_ptr<utils::json_value> json() const
		{
			auto json = utils::json_value::create<utils::json_value_kind::OBJECT>();
			json->obj_add_num("line", (double)line);
			json->obj_add_num("character", (double)character);
			return json;
		}
	};

	struct text_document_identifier {
		std::string uri;

		static std::optional<text_document_identifier> from_json(const std::shared_ptr<utils::json_value> &json)
		{
			using namespace ::hill::utils;

			if (json->kind()!=json_value_kind::OBJECT) return std::nullopt;

			if (!json->obj_has("uri")) return std::nullopt;
			auto uri_json = json->obj_get("uri").value();
			if (uri_json->kind()!=json_value_kind::STRING) return std::nullopt;

			return text_document_identifier{.uri = uri_json->str().value()};
		}

		std::shared_ptr<utils::json_value> json() const
		{
			auto json = utils::json_value::create<utils::json_value_kind::OBJECT>();
			json->obj_add_str("uri", uri);
			return json;
		}
	};

	struct text_document_position_params {
		text_document_identifier text_document;
		models::position position;
		//TODO: context

		static std::optional<text_document_position_params> from_json(const std::shared_ptr<utils::json_value> &json)
		{
			using namespace ::hill::utils;

			if (json->kind()!=json_value_kind::OBJECT) return std::nullopt;
			if (!json->obj_has("textDocument")) return std::nullopt;
			if (!json->obj_has("position")) return std::nullopt;

			auto text_document = text_document_identifier::from_json(json->obj_get("textDocument").value());
			if (!text_document.has_value()) return std::nullopt;
			auto position = models::position::from_json(json->obj_get("position").value());
			if (!position.has_value()) return std::nullopt;

			return text_document_position_params{
				.text_document = text_document.value(),
				.position = position.value(),
			};
		}

		std::shared_ptr<utils::json_value> json() const
		{
			auto json = utils::json_value::create<utils::json_value_kind::OBJECT>();
			json->obj_add_obj("textDocument", text_document.json());
			json->obj_add_obj("position", position.json());
			return json;
		}
	};

	struct completion_item {
		std::string label;

#if 0
		labelDetails?: CompletionItemLabelDetails;
		kind?: CompletionItemKind;
		tags?: CompletionItemTag[];
		detail?: string;
		documentation?: string | MarkupContent;
		deprecated?: boolean;
		preselect?: boolean;
		sortText?: string;
		filterText?: string;
		insertText?: string;
		insertTextFormat?: InsertTextFormat;
		insertTextMode?: InsertTextMode;
		textEdit?: TextEdit | InsertReplaceEdit;
		textEditText?: string;
		additionalTextEdits?: TextEdit[];
		commitCharacters?: string[];
		command?: Command;
		data?: LSPAny;
#endif
		std::shared_ptr<utils::json_value> json() const
		{
			auto json = utils::json_value::create<utils::json_value_kind::OBJECT>();

			json->obj_add_str("label", label);

			return json;
		}
	};

	/**
	 * Represents a collection of [completion items](#CompletionItem) to be
	 * presented in the editor.
	 */
	struct completion_list {
		bool is_incomplete;

#if 0
		itemDefaults ? : {
			commitCharacters ? : string[];
			editRange ? : Range | {
				insert: Range;
				replace: Range;
			};
			insertTextFormat ? : InsertTextFormat;
			insertTextMode ? : InsertTextMode;
			data ? : LSPAny;
		}
#endif
		/**
		 * The completion items.
		 */
		std::vector<completion_item> items;

		std::shared_ptr<utils::json_value> json() const
		{
			auto json = utils::json_value::create<utils::json_value_kind::OBJECT>();

			json->obj_add_bool("isIncomplete", is_incomplete);

			auto itemsArr = json->obj_add_arr("items").value();
			for (const auto &item : items) {
				itemsArr->arr_add_obj(item.json());
			}

			return json;
		}
	};

	struct versioned_text_document_identifier {
		std::string uri;
		int version;

		static std::optional<versioned_text_document_identifier> from_json(const std::shared_ptr<utils::json_value> &json)
		{
			using namespace ::hill::utils;

			if (!json->obj_has("uri")) return std::nullopt;
			auto uri_json = json->obj_get("uri").value();
			if (uri_json->kind() != json_value_kind::STRING) return std::nullopt;

			if (!json->obj_has("version")) return std::nullopt;
			auto version_json = json->obj_get("version").value();
			if (version_json->kind()!=json_value_kind::NUMBER) return std::nullopt;
			
			return versioned_text_document_identifier{
				.uri = uri_json->str().value(),
				.version = (int)version_json->num().value(),
			};
		}

		std::shared_ptr<utils::json_value> json() const
		{
			auto json = utils::json_value::create<utils::json_value_kind::OBJECT>();
			return json;
		}
	};

	/*struct text_document_content_change_event {
#if 0
		range: Range;
#endif
		std::optional<uint32_t> range_length; // rangeLength
		std::string text;

		static std::optional<text_document_content_change_event> from_json(const std::shared_ptr<utils::json_value> &json)
		{
			using namespace ::hill::utils;

			if (json->kind()!=json_value_kind::OBJECT) return std::nullopt;
			
			return text_document_content_change_event{};
		}

		std::shared_ptr<utils::json_value> json() const
		{
			auto json = utils::json_value::create<utils::json_value_kind::OBJECT>();
			return json;
		}
	};*/

	struct text_document_content_change_all {
		std::string text;

		static std::optional<text_document_content_change_all> from_json(const std::shared_ptr<utils::json_value> &json)
		{
			using namespace ::hill::utils;

			if (json->kind()!=json_value_kind::OBJECT) return std::nullopt;
			if (!json->obj_has("text")) return std::nullopt;
			auto text_json = json->obj_get("text").value();
			if (text_json->kind()!=json_value_kind::STRING) return std::nullopt;

			return text_document_content_change_all{
				.text = text_json->str().value()
			};
		}

		std::shared_ptr<utils::json_value> json() const
		{
			auto json = utils::json_value::create<utils::json_value_kind::OBJECT>();
			json->obj_add_str("text", text);
			return json;
		}
	};

	struct did_change_text_document_params {
		versioned_text_document_identifier text_document;
		// TODO: Use variant
		//std::vector<text_document_content_change_event> content_changes;
		//std::variant<std::vector<text_document_content_change_event>, std::vector<text_document_content_change_all>> content_changes;
		std::vector<text_document_content_change_all> content_changes;

		static std::optional<did_change_text_document_params> from_json(const std::shared_ptr<utils::json_value> &json)
		{
			using namespace ::hill::utils;

			if (json->kind()!=json_value_kind::OBJECT) return std::nullopt;

			if (!json->obj_has("textDocument")) return std::nullopt;
			auto text_document = versioned_text_document_identifier::from_json(json->obj_get("textDocument").value());
			if (!text_document.has_value()) return std::nullopt;

			if (!json->obj_has("contentChanges")) return std::nullopt;
			auto content_changes_arr = json->obj_get("contentChanges").value();
			if (content_changes_arr->kind()!=json_value_kind::ARRAY) return std::nullopt;

			std::vector<text_document_content_change_all> content_changes;
			for (const auto &el : *content_changes_arr->arr()) {
				auto content_change = models::text_document_content_change_all::from_json(el);
				if (!content_change.has_value()) return std::nullopt;
				content_changes.push_back(content_change.value());
			}

			//auto content_changes = models::text_document_content_change_all::from_json(json->obj_get("contentChanges").value());
			//if (!content_changes.has_value()) return std::nullopt;

			return did_change_text_document_params{
				.text_document = text_document.value(),
				.content_changes = content_changes,
			};
		}

		std::shared_ptr<utils::json_value> json() const
		{
			auto json = utils::json_value::create<utils::json_value_kind::OBJECT>();

			json->obj_add_obj("textDocument", text_document.json());

			auto content_changes_arr = json->obj_add_arr("contentChanges").value();
			for (const auto &content_change : content_changes) {
				content_changes_arr->arr_add_obj(content_change.json());
			}

			return json;
		}
	};
};

#endif /* HILL__LSP__MODELS_HPP_INCLUDED */
