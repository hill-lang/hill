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
		default: throw internal_exception();
		}
	}

	constexpr std::optional<method> method_parse(const std::string &str)
	{
		if (str==method_str(method::INITIALIZE)) return method::INITIALIZE;
		else if (str==method_str(method::INITIALIZED)) return method::INITIALIZED;
		else if (str==method_str(method::TEXT_DOCUMENT_COMPLETION)) return method::TEXT_DOCUMENT_COMPLETION;
		else return std::nullopt;
	}

	struct request_message {
		/**
		 * The request id.
		 */
		int id;

		/**
		 * The method to be invoked.
		 */
		models::method method;

		/**
		 * The method's params.
		 */
		std::optional<std::shared_ptr<::hill::utils::json_value>> params;
	};

	struct notification_message {
		/**
		 * The method to be invoked.
		 */
		models::method method;

		/**
		 * The method's params.
		 */
		std::optional<std::shared_ptr<::hill::utils::json_value>> params;
	};

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

	struct response_error {
		/**
		 * A number indicating the error type that occurred.
		 */
		error_code code;

		/**
		 * A string providing a short description of the error.
		 */
		std::string message;

		/**
		 * A primitive or structured value that contains additional
		 * information about the error. Can be omitted.
		 */
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
		/**
		 * The request id.
		 */
		int id;

		/**
		 * The result of a request. This member is REQUIRED on success.
		 * This member MUST NOT exist if there was an error invoking the method.
		 */
		std::optional<result_t> result;

		/**
		 * The error object in case a request fails.
		 */
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

	struct completion_options {

		std::shared_ptr<utils::json_value> json() const
		{
			auto json = utils::json_value::create<utils::json_value_kind::OBJECT>();

			return json;
		}
	};

	/**
	 * The capabilities the language server provides.
	 */
	struct server_capabilities {

		/**
		 * The server provides completion support.
		 */
		std::optional<completion_options> completion_provider;

		std::shared_ptr<utils::json_value> json() const
		{
			auto json = utils::json_value::create<utils::json_value_kind::OBJECT>();

			if (completion_provider.has_value()) {
				json->obj_add_obj("completionProvider", completion_provider.value().json());
			}
			
			return json;
		}
	};

	/**
	 * Information about the server
	 */
	struct server_info {
		/**
		 * The name of the server as defined by the server.
		 */
		std::string name;

		/**
		 * The server's version as defined by the server.
		 */
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
		/**
		 * The label of this completion item.
		 *
		 * The label property is also by default the text that
		 * is inserted when selecting this completion.
		 *
		 * If label details are provided the label itself should
		 * be an unqualified name of the completion item.
		 */
		std::string label;

#if 0
		/**
		 * Additional details for the label
		 *
		 * @since 3.17.0
		 */
		labelDetails?: CompletionItemLabelDetails;


		/**
		 * The kind of this completion item. Based of the kind
		 * an icon is chosen by the editor. The standardized set
		 * of available values is defined in `CompletionItemKind`.
		 */
		kind?: CompletionItemKind;

		/**
		 * Tags for this completion item.
		 *
		 * @since 3.15.0
		 */
		tags?: CompletionItemTag[];

		/**
		 * A human-readable string with additional information
		 * about this item, like type or symbol information.
		 */
		detail?: string;

		/**
		 * A human-readable string that represents a doc-comment.
		 */
		documentation?: string | MarkupContent;

		/**
		 * Indicates if this item is deprecated.
		 *
		 * @deprecated Use `tags` instead if supported.
		 */
		deprecated?: boolean;

		/**
		 * Select this item when showing.
		 *
		 * *Note* that only one completion item can be selected and that the
		 * tool / client decides which item that is. The rule is that the *first*
		 * item of those that match best is selected.
		 */
		preselect?: boolean;

		/**
		 * A string that should be used when comparing this item
		 * with other items. When omitted the label is used
		 * as the sort text for this item.
		 */
		sortText?: string;

		/**
		 * A string that should be used when filtering a set of
		 * completion items. When omitted the label is used as the
		 * filter text for this item.
		 */
		filterText?: string;

		/**
		 * A string that should be inserted into a document when selecting
		 * this completion. When omitted the label is used as the insert text
		 * for this item.
		 *
		 * The `insertText` is subject to interpretation by the client side.
		 * Some tools might not take the string literally. For example
		 * VS Code when code complete is requested in this example
		 * `con<cursor position>` and a completion item with an `insertText` of
		 * `console` is provided it will only insert `sole`. Therefore it is
		 * recommended to use `textEdit` instead since it avoids additional client
		 * side interpretation.
		 */
		insertText?: string;

		/**
		 * The format of the insert text. The format applies to both the
		 * `insertText` property and the `newText` property of a provided
		 * `textEdit`. If omitted defaults to `InsertTextFormat.PlainText`.
		 *
		 * Please note that the insertTextFormat doesn't apply to
		 * `additionalTextEdits`.
		 */
		insertTextFormat?: InsertTextFormat;

		/**
		 * How whitespace and indentation is handled during completion
		 * item insertion. If not provided the client's default value depends on
		 * the `textDocument.completion.insertTextMode` client capability.
		 *
		 * @since 3.16.0
		 * @since 3.17.0 - support for `textDocument.completion.insertTextMode`
		 */
		insertTextMode?: InsertTextMode;

		/**
		 * An edit which is applied to a document when selecting this completion.
		 * When an edit is provided the value of `insertText` is ignored.
		 *
		 * *Note:* The range of the edit must be a single line range and it must
		 * contain the position at which completion has been requested.
		 *
		 * Most editors support two different operations when accepting a completion
		 * item. One is to insert a completion text and the other is to replace an
		 * existing text with a completion text. Since this can usually not be
		 * predetermined by a server it can report both ranges. Clients need to
		 * signal support for `InsertReplaceEdit`s via the
		 * `textDocument.completion.completionItem.insertReplaceSupport` client
		 * capability property.
		 *
		 * *Note 1:* The text edit's range as well as both ranges from an insert
		 * replace edit must be a [single line] and they must contain the position
		 * at which completion has been requested.
		 * *Note 2:* If an `InsertReplaceEdit` is returned the edit's insert range
		 * must be a prefix of the edit's replace range, that means it must be
		 * contained and starting at the same position.
		 *
		 * @since 3.16.0 additional type `InsertReplaceEdit`
		 */
		textEdit?: TextEdit | InsertReplaceEdit;

		/**
		 * The edit text used if the completion item is part of a CompletionList and
		 * CompletionList defines an item default for the text edit range.
		 *
		 * Clients will only honor this property if they opt into completion list
		 * item defaults using the capability `completionList.itemDefaults`.
		 *
		 * If not provided and a list's default range is provided the label
		 * property is used as a text.
		 *
		 * @since 3.17.0
		 */
		textEditText?: string;

		/**
		 * An optional array of additional text edits that are applied when
		 * selecting this completion. Edits must not overlap (including the same
		 * insert position) with the main edit nor with themselves.
		 *
		 * Additional text edits should be used to change text unrelated to the
		 * current cursor position (for example adding an import statement at the
		 * top of the file if the completion item will insert an unqualified type).
		 */
		additionalTextEdits?: TextEdit[];

		/**
		 * An optional set of characters that when pressed while this completion is
		 * active will accept it first and then type that character. *Note* that all
		 * commit characters should have `length=1` and that superfluous characters
		 * will be ignored.
		 */
		commitCharacters?: string[];

		/**
		 * An optional command that is executed *after* inserting this completion.
		 * *Note* that additional modifications to the current document should be
		 * described with the additionalTextEdits-property.
		 */
		command?: Command;

		/**
		 * A data entry field that is preserved on a completion item between
		 * a completion and a completion resolve request.
		 */
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
		/**
		 * This list is not complete. Further typing should result in recomputing
		 * this list.
		 *
		 * Recomputed lists have all their items replaced (not appended) in the
		 * incomplete completion sessions.
		 */
		bool is_incomplete;

#if 0
		/**
		 * In many cases the items of an actual completion result share the same
		 * value for properties like `commitCharacters` or the range of a text
		 * edit. A completion list can therefore define item defaults which will
		 * be used if a completion item itself doesn't specify the value.
		 *
		 * If a completion list specifies a default value and a completion item
		 * also specifies a corresponding value the one from the item is used.
		 *
		 * Servers are only allowed to return default values if the client
		 * signals support for this via the `completionList.itemDefaults`
		 * capability.
		 *
		 * @since 3.17.0
		 */
		itemDefaults ? : {
			/**
			 * A default commit character set.
			 *
			 * @since 3.17.0
			 */
			commitCharacters ? : string[];

			/**
			 * A default edit range
			 *
			 * @since 3.17.0
			 */
			editRange ? : Range | {
			insert: Range;
			replace: Range;
			};

			/**
			 * A default insert text format
			 *
			 * @since 3.17.0
			 */
			insertTextFormat ? : InsertTextFormat;

			/**
			 * A default insert text mode
			 *
			 * @since 3.17.0
			 */
			insertTextMode ? : InsertTextMode;

			/**
			 * A default data value.
			 *
			 * @since 3.17.0
			 */
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
};

#endif /* HILL__LSP__MODELS_HPP_INCLUDED */
