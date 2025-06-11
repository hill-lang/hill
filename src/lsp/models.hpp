#ifndef HILL__LSP__MODELS_HPP_INCLUDED
#define HILL__LSP__MODELS_HPP_INCLUDED

#include "../exceptions.hpp"
#include "../utils/json.hpp"

#include <optional>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdint.h>

namespace hill::lsp::models {

	enum class method {
		// Lifecycle
		INITIALIZE,
		INITIALIZED,
		// TODO: Register capability
		// TODO: Unregister capability
		SET_TRACE,
		LOG_TRACE,
		SHUTDOWN,
		EXIT,
		// Document Synchronization
		TEXT_DOCUMENT_DID_OPEN,
		TEXT_DOCUMENT_DID_CHANGE,
		TEXT_DOCUMENT_WILL_SAVE,
		TEXT_DOCUMENT_WILL_SAVE_WAIT_UNTIL,
		TEXT_DOCUMENT_DID_SAVE,
		TEXT_DOCUMENT_DID_CLOSE,
		WORKSPACE_DOCUMENT_DID_RENAME,
		// Text document
		TEXT_DOCUMENT_COMPLETION,
	};

	constexpr const char *method_str(method m)
	{
		switch (m) {
		case method::INITIALIZE: return "initialize";
		case method::INITIALIZED: return "initialized";
		case method::SET_TRACE: return "$/setTrace";
		case method::LOG_TRACE: return "$/logTrace";
		case method::SHUTDOWN: return "shutdown";
		case method::EXIT: return "exit";
		case method::TEXT_DOCUMENT_DID_OPEN: return "textDocument/didOpen";
		case method::TEXT_DOCUMENT_DID_CHANGE: return "textDocument/didChange";
		case method::TEXT_DOCUMENT_WILL_SAVE: return "textDocument/willSave";
		case method::TEXT_DOCUMENT_WILL_SAVE_WAIT_UNTIL: return "textDocument/willSaveWaitUntil";
		case method::TEXT_DOCUMENT_DID_SAVE: return "textDocument/didSave";
		case method::TEXT_DOCUMENT_DID_CLOSE: return "textDocument/didClose";
		case method::WORKSPACE_DOCUMENT_DID_RENAME: return "workspace/didRenameFiles";
		case method::TEXT_DOCUMENT_COMPLETION: return "textDocument/completion";
		default: throw internal_exception();
		}
	}

	constexpr std::optional<method> method_parse(const std::string &str)
	{
		if (str==method_str(method::INITIALIZE)) return method::INITIALIZE;
		else if (str==method_str(method::INITIALIZED)) return method::INITIALIZED;
		else if (str==method_str(method::SET_TRACE)) return method::SET_TRACE;
		else if (str==method_str(method::LOG_TRACE)) return method::LOG_TRACE;
		else if (str==method_str(method::SHUTDOWN)) return method::SHUTDOWN;
		else if (str==method_str(method::EXIT)) return method::EXIT;
		else if (str==method_str(method::TEXT_DOCUMENT_DID_OPEN)) return method::TEXT_DOCUMENT_DID_OPEN;
		else if (str==method_str(method::TEXT_DOCUMENT_DID_CHANGE)) return method::TEXT_DOCUMENT_DID_CHANGE;
		else if (str==method_str(method::TEXT_DOCUMENT_WILL_SAVE)) return method::TEXT_DOCUMENT_WILL_SAVE;
		else if (str==method_str(method::TEXT_DOCUMENT_WILL_SAVE_WAIT_UNTIL)) return method::TEXT_DOCUMENT_WILL_SAVE_WAIT_UNTIL;
		else if (str==method_str(method::TEXT_DOCUMENT_DID_SAVE)) return method::TEXT_DOCUMENT_DID_SAVE;
		else if (str==method_str(method::TEXT_DOCUMENT_DID_CLOSE)) return method::TEXT_DOCUMENT_DID_CLOSE;
		else if (str==method_str(method::WORKSPACE_DOCUMENT_DID_RENAME)) return method::WORKSPACE_DOCUMENT_DID_RENAME;
		else if (str==method_str(method::TEXT_DOCUMENT_COMPLETION)) return method::TEXT_DOCUMENT_COMPLETION;
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
		std::optional<std::shared_ptr<::hill::utils::json_value>> params = {};
	};

	struct response_error {
		error_code code;
		std::string message;
		std::optional<std::shared_ptr<::hill::utils::json_value>> data = {};

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
		std::optional<result_t> result = {};
		std::optional<response_error> error = {};

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

	struct notification_message {
		models::method method;
		std::optional<std::shared_ptr<::hill::utils::json_value>> params = {};
	};

	struct cancel_params {
		/**
		 * The request id to cancel
		 */
		int id;

		static std::optional<cancel_params> from_json(const std::shared_ptr<utils::json_value> &json)
		{
			using namespace ::hill::utils;

			if (json->kind()!=json_value_kind::OBJECT) return std::nullopt;
			if (!json->obj_has("id")) return std::nullopt;
			auto id = json->obj_get("id").value();
			if (id->kind()!=json_value_kind::NUMBER) return std::nullopt;

			return cancel_params{.id = (int)id->num().value()};
		}

		std::shared_ptr<utils::json_value> json() const
		{
			auto json = utils::json_value::create<utils::json_value_kind::OBJECT>();
			json->obj_add_num("id", (double)id);
			return json;
		}
	};

	struct progress_params {
		/**
		 * The progress token provided by the client or server.
		 */
		int token;

		/**
		 * The progress data.
		 */
		std::shared_ptr<::hill::utils::json_value> value;
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

	struct range {
		position start;
		position end;

		static std::optional<range> from_json(const std::shared_ptr<utils::json_value> &json)
		{
			using namespace ::hill::utils;

			if (json->kind()!=json_value_kind::OBJECT) return std::nullopt;
			if (!json->obj_has("start")) return std::nullopt;

			auto start = position::from_json(json->obj_get("start").value());
			if (!start.has_value()) return std::nullopt;

			auto end = position::from_json(json->obj_get("end").value());
			if (!end.has_value()) return std::nullopt;

			return range{
				.start = start.value(),
				.end = end.value(),
			};
		}

		std::shared_ptr<utils::json_value> json() const
		{
			auto json = utils::json_value::create<utils::json_value_kind::OBJECT>();
			json->obj_add_obj("start", start.json());
			json->obj_add_obj("end", end.json());
			return json;
		}
	};

	struct text_document_item
	{
		std::string uri;
		std::string language_id;
		int version;
		std::string text;

		static std::optional<text_document_item> from_json(const std::shared_ptr<utils::json_value> &json)
		{
			using namespace ::hill::utils;

			if (json->kind()!=json_value_kind::OBJECT) return std::nullopt;

			if (!json->obj_has("uri")) return std::nullopt;
			auto uri = json->obj_get("uri").value();
			if (uri->kind()!=json_value_kind::STRING) return std::nullopt;

			if (!json->obj_has("languageId")) return std::nullopt;
			auto language_id = json->obj_get("languageId").value();
			if (language_id->kind()!=json_value_kind::STRING) return std::nullopt;

			if (!json->obj_has("version")) return std::nullopt;
			auto version = json->obj_get("version").value();
			if (version->kind()!=json_value_kind::NUMBER) return std::nullopt;

			if (!json->obj_has("text")) return std::nullopt;
			auto text = json->obj_get("text").value();
			if (text->kind()!=json_value_kind::STRING) return std::nullopt;

			return text_document_item{
				.uri = uri->str().value(),
				.language_id = language_id->str().value(),
				.version = (int)version->num().value(),
				.text = text->str().value(),
			};
		}

		std::shared_ptr<utils::json_value> json() const
		{
			auto json = utils::json_value::create<utils::json_value_kind::OBJECT>();
			json->obj_add_str("uri", uri);
			json->obj_add_str("languageId", language_id);
			json->obj_add_num("version", (double)version);
			json->obj_add_str("text", text);
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

	struct versioned_text_document_identifier {
		std::string uri;
		int version;

		static std::optional<versioned_text_document_identifier> from_json(const std::shared_ptr<utils::json_value> &json)
		{
			using namespace ::hill::utils;

			if (!json->obj_has("uri")) return std::nullopt;
			auto uri_json = json->obj_get("uri").value();
			if (uri_json->kind()!=json_value_kind::STRING) return std::nullopt;

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
			json->obj_add_str("uri", uri);
			json->obj_add_num("version", (double)version);
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

	struct document_filter {
		std::optional<std::string> language = {};
		std::optional<std::string> scheme = {};
		std::optional<std::string> pattern = {};

		static std::optional<document_filter> from_json(const std::shared_ptr<utils::json_value> &json)
		{
			using namespace ::hill::utils;

			if (json->kind()!=json_value_kind::OBJECT) return std::nullopt;

			std::optional<std::string> language = std::nullopt;
			std::optional<std::string> scheme = std::nullopt;
			std::optional<std::string> pattern = std::nullopt;

			auto language_json = json->obj_get("language");
			if (language_json.has_value()) {
				if (language_json.value()->kind()!=json_value_kind::STRING) return std::nullopt;
				else language = language_json.value()->str();
			}

			auto scheme_json = json->obj_get("scheme");
			if (scheme_json.has_value()) {
				if (scheme_json.value()->kind()!=json_value_kind::STRING) return std::nullopt;
				else scheme = scheme_json.value()->str();
			}

			auto pattern_json = json->obj_get("pattern");
			if (pattern_json.has_value()) {
				if (pattern_json.value()->kind()!=json_value_kind::STRING) return std::nullopt;
				else pattern = pattern_json.value()->str();
			}

			return document_filter{
				.language = language,
				.scheme = scheme,
				.pattern = pattern,
			};
		}

		std::shared_ptr<utils::json_value> json() const
		{
			auto json = utils::json_value::create<utils::json_value_kind::OBJECT>();
			if (language.has_value()) {json->obj_add_str("language", language.value());}
			if (scheme.has_value()) {json->obj_add_str("scheme", scheme.value());}
			if (pattern.has_value()) {json->obj_add_str("pattern", pattern.value());}
			return json;
		}
	};

	struct text_edit {
		models::range range;
		std::string new_text;

		static std::optional<text_edit> from_json(const std::shared_ptr<utils::json_value> &json)
		{
			using namespace ::hill::utils;

			if (json->kind()!=json_value_kind::OBJECT) return std::nullopt;

			if (!json->obj_has("range")) return std::nullopt;
			auto range = models::range::from_json(json->obj_get("range").value());
			if (!range.has_value()) return std::nullopt;

			if (!json->obj_has("newText")) return std::nullopt;
			auto new_text = json->obj_get("newText").value();
			if (new_text->kind()!=json_value_kind::STRING) return std::nullopt;

			return text_edit{
				.range = range.value(),
				.new_text = new_text->str().value(),
			};
		}

		std::shared_ptr<utils::json_value> json() const
		{
			auto json = utils::json_value::create<utils::json_value_kind::OBJECT>();
			json->obj_add_obj("range", range.json());
			json->obj_add_str("newText", new_text);
			return json;
		}
	};

	struct change_annotation {
		std::string label;
		std::optional<bool> needs_confirmation = {};
		std::optional<std::string> description = {};

		static std::optional<change_annotation> from_json(const std::shared_ptr<utils::json_value> &json)
		{
			using namespace ::hill::utils;

			if (json->kind()!=json_value_kind::OBJECT) return std::nullopt;

			if (!json->obj_has("label")) return std::nullopt;
			auto label = json->obj_get("label").value();
			if (label->kind()!=json_value_kind::STRING) return std::nullopt;

			std::optional<bool> needs_confirmation = {};
			std::optional<std::string> description = {};

			auto needs_confirmation_json = json->obj_get("needsConfirmation");
			if (needs_confirmation_json.has_value()) {
				if (needs_confirmation_json.value()->kind()!=json_value_kind::BOOL) return std::nullopt;
				else needs_confirmation = needs_confirmation_json.value()->boolean();
			}

			auto description_json = json->obj_get("description");
			if (description_json.has_value()) {
				if (description_json.value()->kind()!=json_value_kind::STRING) return std::nullopt;
				else description = description_json.value()->str();
			}

			return change_annotation{
				.label = label->str().value(),
				.needs_confirmation = needs_confirmation,
				.description = description,
			};
		}

		std::shared_ptr<utils::json_value> json() const
		{
			auto json = utils::json_value::create<utils::json_value_kind::OBJECT>();
			json->obj_add_str("label", label);
			if (needs_confirmation.has_value()) json->obj_add_bool("needsConfirmation", needs_confirmation.value());
			if (needs_confirmation.has_value()) json->obj_add_str("description", description.value());
			return json;
		}
	};

	struct annotated_text_edit {
		models::range range;
		std::string new_text;
		std::string annotation_id;

		static std::optional<annotated_text_edit> from_json(const std::shared_ptr<utils::json_value> &json)
		{
			using namespace ::hill::utils;

			if (json->kind()!=json_value_kind::OBJECT) return std::nullopt;

			if (!json->obj_has("range")) return std::nullopt;
			auto range = models::range::from_json(json->obj_get("range").value());
			if (!range.has_value()) return std::nullopt;

			if (!json->obj_has("newText")) return std::nullopt;
			auto new_text = json->obj_get("newText").value();
			if (new_text->kind()!=json_value_kind::STRING) return std::nullopt;

			if (!json->obj_has("annotationId")) return std::nullopt;
			auto annotation_id = json->obj_get("annotationId").value();
			if (annotation_id->kind()!=json_value_kind::STRING) return std::nullopt;

			return annotated_text_edit{
				.range = range.value(),
				.new_text = new_text->str().value(),
				.annotation_id = annotation_id->str().value(),
			};
		}

		std::shared_ptr<utils::json_value> json() const
		{
			auto json = utils::json_value::create<utils::json_value_kind::OBJECT>();
			json->obj_add_obj("range", range.json());
			json->obj_add_str("newText", new_text);
			json->obj_add_str("annotationId", annotation_id);
			return json;
		}
	};

	struct text_document_edit {
		// TODO:
	};

	struct location {
		std::string uri;
		models::range range;

		static std::optional<location> from_json(const std::shared_ptr<utils::json_value> &json)
		{
			using namespace ::hill::utils;

			if (json->kind()!=json_value_kind::OBJECT) return std::nullopt;

			if (!json->obj_has("uri")) return std::nullopt;
			auto uri = json->obj_get("uri").value();
			if (uri->kind()!=json_value_kind::STRING) return std::nullopt;

			if (!json->obj_has("range")) return std::nullopt;
			auto range = models::range::from_json(json->obj_get("range").value());
			if (!range.has_value()) return std::nullopt;

			return location{
				.uri = uri->str().value(),
				.range = range.value(),
			};
		}

		std::shared_ptr<utils::json_value> json() const
		{
			auto json = utils::json_value::create<utils::json_value_kind::OBJECT>();
			json->obj_add_str("uri", uri);
			json->obj_add_obj("range", range.json());
			return json;
		}
	};

	struct location_link {
		// TODO:
	};

	enum class diagnostic_severity: int {
		ERROR = 1,
		WARNING = 2,
		INFORMATION = 3,
		HINT = 4,
	};

	enum class diagnostic_tag {
		/**
		 * Unused or unnecessary code.
		 *
		 * Clients are allowed to render diagnostics with this tag faded out
		 * instead of having an error squiggle.
		 */
		UNNECESSARY = 1,
		/**
		 * Deprecated or obsolete code.
		 *
		 * Clients are allowed to rendered diagnostics with this tag strike through.
		 */
		DEPRECATED = 2,
	};

	struct diagnostic_related_information {
		models::location location;
		std::string message;

		static std::optional<diagnostic_related_information> from_json(const std::shared_ptr<utils::json_value> &json)
		{
			using namespace ::hill::utils;

			if (json->kind()!=json_value_kind::OBJECT) return std::nullopt;

			if (!json->obj_has("location")) return std::nullopt;
			auto location = models::location::from_json(json->obj_get("location").value());
			if (!location.has_value()) return std::nullopt;

			if (!json->obj_has("message")) return std::nullopt;
			auto message = json->obj_get("message").value();
			if (message->kind()!=json_value_kind::STRING) return std::nullopt;

			return diagnostic_related_information{
				.location = location.value(),
				.message = message->str().value(),
			};
		}

		std::shared_ptr<utils::json_value> json() const
		{
			auto json = utils::json_value::create<utils::json_value_kind::OBJECT>();
			json->obj_add_str("message", message);
			return json;
		}
	};

	struct code_description {
		std::string uri;

		static std::optional<code_description> from_json(const std::shared_ptr<utils::json_value> &json)
		{
			using namespace ::hill::utils;

			if (json->kind()!=json_value_kind::OBJECT) return std::nullopt;

			if (!json->obj_has("uri")) return std::nullopt;
			auto uri = json->obj_get("uri").value();
			if (uri->kind()!=json_value_kind::STRING) return std::nullopt;

			return code_description{.uri = uri->str().value()};
		}

		std::shared_ptr<utils::json_value> json() const
		{
			auto json = utils::json_value::create<utils::json_value_kind::OBJECT>();
			json->obj_add_str("uri", uri);
			return json;
		}
	};

	struct diagnostic {
		models::range range;
		std::optional<diagnostic_severity> severity = {};
		std::optional<int> code = {}; // TODO: Int or string?
		std::optional<models::code_description> code_description = {};
		std::optional<std::string> source = {};
		std::string message;
		std::optional<std::vector<diagnostic_tag>> tags = {};
		std::optional<std::vector<diagnostic_related_information>> related_information = {};
		std::optional<std::shared_ptr<::hill::utils::json_value>> data = {};

		// TODO: parsing and serializing
	};

	struct command {
		std::string title;
		std::string command;
		std::optional<std::vector<std::shared_ptr<utils::json_value>>> arguments = {};

		// TODO: parsing

		std::shared_ptr<utils::json_value> json() const
		{
			auto json = utils::json_value::create<utils::json_value_kind::OBJECT>();
			json->obj_add_str("title", title);
			json->obj_add_str("command", command);
			if (arguments.has_value()) {
				auto arr = json->obj_add_arr("arguments").value();
				for (const auto &arg : arguments.value()) {
					arr->arr_add_obj(arg);
				}
			}
			return json;
		}
	};

	enum class markup_kind {
		PLAIN_TEXT,
		MARKDOWN,
	};

	constexpr const char *markup_kind_str(markup_kind kind)
	{
		switch (kind) {
		case markup_kind::PLAIN_TEXT: return "plaintext";
		case markup_kind::MARKDOWN: return "markdown";
		default: throw internal_exception();
		}
	}

	inline std::optional<markup_kind> markup_kind_parse(const std::string &str)
	{
		if (str==markup_kind_str(markup_kind::PLAIN_TEXT)) return markup_kind::PLAIN_TEXT;
		else if (str==markup_kind_str(markup_kind::MARKDOWN)) return markup_kind::MARKDOWN;
		else return std::nullopt;
	}

	struct markup_content {
		markup_kind kind;
		std::string value;

		static std::optional<markup_content> from_json(const std::shared_ptr<utils::json_value> &json)
		{
			using namespace ::hill::utils;

			if (json->kind()!=json_value_kind::OBJECT) return std::nullopt;

			if (!json->obj_has("kind")) return std::nullopt;
			auto kind_str = json->obj_get("kind").value();
			if (kind_str->kind()!=json_value_kind::STRING) return std::nullopt;
			auto kind = markup_kind_parse(kind_str->str().value());
			if (!kind.has_value()) return std::nullopt;

			if (!json->obj_has("value")) return std::nullopt;
			auto value = json->obj_get("value").value();
			if (value->kind() != json_value_kind::STRING) return std::nullopt;

			return markup_content{
				.kind = kind.value(),
				.value = value->str().value(),
			};
		}

		std::shared_ptr<utils::json_value> json() const
		{
			auto json = utils::json_value::create<utils::json_value_kind::OBJECT>();
			json->obj_add_str("value", value);
			return json;
		}
	};

	struct markdown_client_capabilities {
		std::string parser;
		std::optional<std::string> version = {};
		std::optional<std::vector<std::string>> allowed_tags = {};

		// TODO: parsing and serializing
	};

	enum class resource_operation_kind {
		CREATE,
		RENAME,
		DELETE,
	};

	constexpr const char *resource_operation_kind_str(resource_operation_kind kind)
	{
		switch (kind) {
		case resource_operation_kind::CREATE: return "create";
		case resource_operation_kind::RENAME: return "rename";
		case resource_operation_kind::DELETE: return "delete";
		default: throw internal_exception();
		}
	}

	inline std::optional<resource_operation_kind> resource_operation_kind_parse(const std::string &str)
	{
		if (str==resource_operation_kind_str(resource_operation_kind::CREATE)) return resource_operation_kind::CREATE;
		else if (str==resource_operation_kind_str(resource_operation_kind::RENAME)) return resource_operation_kind::RENAME;
		else if (str==resource_operation_kind_str(resource_operation_kind::DELETE)) return resource_operation_kind::DELETE;
		else return std::nullopt;
	}

	struct create_file_options {
		std::optional<bool> overwrite = {};
		std::optional<bool> ignore_if_exists = {};

		// TODO: parsing and serializing
	};

	struct create_file {
		//resource_operation_kind kind; // == CREATE
		std::string uri;
		std::optional<create_file_options> options = {};
		std::optional<std::string> annotation_id = {};

		// TODO: parsing and serializing
	};

	struct rename_file_options {
		std::optional<bool> overwrite = {};
		std::optional<bool> ignore_if_exists = {};

		// TODO: parsing and serializing
	};

	struct rename_file {
		//resource_operation_kind kind; // == RENAME
		std::string old_uri;
		std::string new_uri;
		std::optional<rename_file_options> options = {};
		std::optional<std::string> annotation_id = {};

		// TODO: parsing and serializing
	};

	struct delete_file_options {
		std::optional<bool> recursive = {};
		std::optional<bool> ignore_if_not_exists = {};

		// TODO: parsing and serializing
	};

	struct delete_file {
		//resource_operation_kind kind; // == DELETE
		std::string uri;
		std::optional<delete_file_options> options = {};
		std::optional<std::string> annotation_id = {};

		// TODO: parsing and serializing
	};

	enum class failure_handling_kind {
		ABORT,
		TRANSACTIONAL,
		UNDO,
		TEXT_ONLY_TRANSACTIONAL,
	};

	constexpr const char *failure_handling_kind_str(failure_handling_kind kind)
	{
		switch (kind) {
		case failure_handling_kind::ABORT: return "abort";
		case failure_handling_kind::TRANSACTIONAL: return "transactional";
		case failure_handling_kind::UNDO: return "undo";
		case failure_handling_kind::TEXT_ONLY_TRANSACTIONAL: return "textOnlyTransactional";
		default: throw internal_exception();
		}
	}

	inline std::optional<failure_handling_kind> failure_handling_kind_parse(const std::string &str)
	{
		if (str==failure_handling_kind_str(failure_handling_kind::ABORT)) return failure_handling_kind::ABORT;
		else if (str==failure_handling_kind_str(failure_handling_kind::TRANSACTIONAL)) return failure_handling_kind::TRANSACTIONAL;
		else if (str==failure_handling_kind_str(failure_handling_kind::UNDO)) return failure_handling_kind::UNDO;
		else if (str==failure_handling_kind_str(failure_handling_kind::TEXT_ONLY_TRANSACTIONAL)) return failure_handling_kind::TEXT_ONLY_TRANSACTIONAL;
		else return std::nullopt;
	}

	struct workspace_edit {
		// uri => text_edit[]
		std::optional<std::unordered_map<std::string, std::vector<text_edit>>> changes = {};

#if 0
	documentChanges?: (
		TextDocumentEdit[] |
		(TextDocumentEdit | CreateFile | RenameFile | DeleteFile)[]
	);
#endif
		// annotation_id => change_annotation[]
		std::optional<std::unordered_map<std::string, std::vector<change_annotation>>> change_annotations = {};

		// TODO: parsing and serializing
	};

	struct workspace_edit_client_capabilities_change_annotation_support {
		std::optional<bool> groups_on_label = {};

		// TODO: parsing and serializing
	};

	struct workspace_edit_client_capabilities {
		std::optional<bool> document_changes = {};
		std::optional<std::vector<resource_operation_kind>> resource_operations = {};
		std::optional<failure_handling_kind> failure_handling = {};
		std::optional<bool> normalizes_line_endings = {};
		std::optional<workspace_edit_client_capabilities_change_annotation_support> change_annotation_support = {};

		// TODO: parsing and serializing
	};

	struct workspace_folder {
		std::string uri;
		std::string name;

		static std::optional<workspace_folder> from_json(const std::shared_ptr<utils::json_value> &json)
		{
			using namespace ::hill::utils;

			if (json->kind()!=json_value_kind::OBJECT) return std::nullopt;

			if (!json->obj_has("uri")) return std::nullopt;
			auto uri = json->obj_get("uri").value();
			if (uri->kind()!=json_value_kind::STRING) return std::nullopt;

			if (!json->obj_has("name")) return std::nullopt;
			auto name = json->obj_get("name").value();
			if (name->kind()!=json_value_kind::STRING) return std::nullopt;

			return workspace_folder{
				.uri = uri->str().value(),
				.name = name->str().value(),
			};
		}

		std::shared_ptr<utils::json_value> json() const
		{
			auto json = utils::json_value::create<utils::json_value_kind::OBJECT>();
			json->obj_add_str("uri", uri);
			json->obj_add_str("name", name);
			return json;
		}
	};

	enum class work_done_progress_kind {
		BEGIN,
		REPORT,
		END,
	};

	constexpr const char *work_done_progress_kind_str(work_done_progress_kind kind)
	{
		switch (kind) {
		case work_done_progress_kind::BEGIN: return "begin";
		case work_done_progress_kind::REPORT: return "report";
		case work_done_progress_kind::END: return "end";
		default: throw internal_exception();
		}
	}

	inline std::optional<work_done_progress_kind> work_done_progress_kind_parse(const std::string &str)
	{
		if (str==work_done_progress_kind_str(work_done_progress_kind::BEGIN)) return work_done_progress_kind::BEGIN;
		else if (str==work_done_progress_kind_str(work_done_progress_kind::REPORT)) return work_done_progress_kind::REPORT;
		else if (str==work_done_progress_kind_str(work_done_progress_kind::END)) return work_done_progress_kind::END;
		else return std::nullopt;
	}

	struct work_done_progress_begin {
		//work_done_progress_kind kind; // == BEGIN
		std::string title;
		std::optional<bool> cancellable = {};
		std::optional<std::string> message = {};
		std::optional<uint32_t> percentage = {};

		// TODO: parsing and serializing
	};

	struct work_done_progress_report {
		//work_done_progress_kind kind; // == REPORT
		std::optional<bool> cancellable = {};
		std::optional<std::string> message = {};
		std::optional<uint32_t> percentage = {};

		// TODO: parsing and serializing
	};

	struct work_done_progress_end {
		//work_done_progress_kind kind; // == END
		std::optional<std::string> message = {};

		// TODO: parsing and serializing
	};

	struct work_done_progress_params {
		std::optional<int> work_done_token = {};

		static std::optional<work_done_progress_params> from_json(const std::shared_ptr<utils::json_value> &json)
		{
			using namespace ::hill::utils;

			if (json->kind()!=json_value_kind::OBJECT) return std::nullopt;

			std::optional<int> work_done_token = std::nullopt;
			if (json->obj_has("workDoneToken")) {
				auto work_done_token_json = json->obj_get("workDoneToken").value();
				if (work_done_token_json->kind()!=json_value_kind::NUMBER) return std::nullopt;
				else work_done_token = (int)work_done_token_json->num().value();
			}

			return work_done_progress_params{.work_done_token = work_done_token};
		}

		std::shared_ptr<utils::json_value> json() const
		{
			auto json = utils::json_value::create<utils::json_value_kind::OBJECT>();
			if (work_done_token.has_value()) {
				json->obj_add_num("workDoneToken", (double)work_done_token.value());
			}
			return json;
		}
	};

	struct work_done_progress_options {
		std::optional<bool> work_done_progress = {};

		static std::optional<work_done_progress_options> from_json(const std::shared_ptr<utils::json_value> &json)
		{
			using namespace ::hill::utils;

			if (json->kind()!=json_value_kind::OBJECT) return std::nullopt;

			std::optional<bool> work_done_progress = std::nullopt;
			if (json->obj_has("workDoneProgress")) {
				auto work_done_progress_json = json->obj_get("workDoneProgress").value();
				if (work_done_progress_json->kind()!=json_value_kind::BOOL) return std::nullopt;
				else work_done_progress = work_done_progress_json->boolean().value();
			}

			return work_done_progress_options{.work_done_progress = work_done_progress};
		}

		std::shared_ptr<utils::json_value> json() const
		{
			auto json = utils::json_value::create<utils::json_value_kind::OBJECT>();
			if (work_done_progress.has_value()) {
				json->obj_add_bool("workDoneProgress", work_done_progress.value());
			}
			return json;
		}
	};

	struct partial_result_params {
		std::optional<int> partial_result_token = {}; // int or string?

		// TODO: parsing and serializing
	};

	enum class trace_value {
		OFF,
		MESSAGE,
		VERBOSE,
	};

	constexpr const char *trace_value_str(trace_value value)
	{
		switch (value) {
		case trace_value::OFF: return "off";
		case trace_value::MESSAGE: return "message";
		case trace_value::VERBOSE: return "verbose";
		default: throw internal_exception();
		}
	}

	inline std::optional<trace_value> trace_value_parse(const std::string &str)
	{
		if (str==trace_value_str(trace_value::OFF)) return trace_value::OFF;
		else if (str==trace_value_str(trace_value::MESSAGE)) return trace_value::MESSAGE;
		else if (str==trace_value_str(trace_value::VERBOSE)) return trace_value::VERBOSE;
		else return std::nullopt;
	}

	struct client_info {
		std::string name;
		std::optional<std::string> version = {};

		// TODO: parsing and serializing
	};

	struct client_capabilities {

		// TODO:
	};

	// Extends: work_done_progress_params
	struct initialize_params {
		std::optional<int> work_done_token = {};
		std::optional<int> process_id = {}; // int or null
		models::client_info client_info;
		std::optional<std::string> locale = {};
		std::optional<std::string> root_path = {}; // string or null
		std::optional<std::string> root_uri = {}; // string or null
		std::optional<std::shared_ptr<utils::json_value>> initialization_options = {};
		client_capabilities capabilities;
		std::optional<trace_value> trace = {};
		std::optional<std::vector<workspace_folder>> workspace_folders = {}; // workspace_folder[] or null

		// TODO: parsing and serializing

		/*static std::optional<work_done_progress_params> from_json(const std::shared_ptr<utils::json_value> &json)
		{
			using namespace ::hill::utils;

			if (json->kind()!=json_value_kind::OBJECT) return std::nullopt;

			std::optional<int> work_done_token = std::nullopt;
			if (json->obj_has("workDoneToken")) {
				auto work_done_token_json = json->obj_get("workDoneToken").value();
				if (work_done_token_json->kind()!=json_value_kind::NUMBER) return std::nullopt;
				else work_done_token = (int)work_done_token_json->num().value();
			}

			return work_done_progress_params{.work_done_token = work_done_token};
		}

		std::shared_ptr<utils::json_value> json() const
		{
			auto json = utils::json_value::create<utils::json_value_kind::OBJECT>();
			if (work_done_token.has_value()) {
				json->obj_add_num("workDoneToken", (double)work_done_token.value());
			}
			return json;
		}*/
	};

	// --------------------------------------

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

		// TODO;

		std::shared_ptr<utils::json_value> json() const
		{
			auto json = utils::json_value::create<utils::json_value_kind::OBJECT>();

			return json;
		}
	};

	struct server_capabilities {
		std::optional<text_document_sync_kind> text_document_sync = {};
		std::optional<completion_options> completion_provider = {};

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
		std::optional<std::string> version = {};

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
		std::optional<models::server_info> server_info = {};

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

	struct did_open_text_document_params {
		text_document_item text_document;

		static std::optional<did_open_text_document_params> from_json(const std::shared_ptr<utils::json_value> &json)
		{
			using namespace ::hill::utils;

			if (json->kind()!=json_value_kind::OBJECT) return std::nullopt;
			if (!json->obj_has("textDocument")) return std::nullopt;

			auto text_document = text_document_item::from_json(json->obj_get("textDocument").value());
			if (!text_document.has_value()) return std::nullopt;

			return did_open_text_document_params{.text_document=text_document.value()};
		}

		std::shared_ptr<utils::json_value> json() const
		{
			auto json = utils::json_value::create<utils::json_value_kind::OBJECT>();
			json->obj_add_obj("textDocument", text_document.json());
			return json;
		}
	};

	struct did_close_text_document_params {
		text_document_identifier text_document;

		static std::optional<did_close_text_document_params> from_json(const std::shared_ptr<utils::json_value> &json)
		{
			using namespace ::hill::utils;

			if (json->kind()!=json_value_kind::OBJECT) return std::nullopt;
			if (!json->obj_has("textDocument")) return std::nullopt;

			auto text_document = text_document_identifier::from_json(json->obj_get("textDocument").value());
			if (!text_document.has_value()) return std::nullopt;

			return did_close_text_document_params{.text_document=text_document.value()};
		}

		std::shared_ptr<utils::json_value> json() const
		{
			auto json = utils::json_value::create<utils::json_value_kind::OBJECT>();
			json->obj_add_obj("textDocument", text_document.json());
			return json;
		}
	};

	struct completion_item_label_details {
		std::optional<std::string> detail = {};
		std::optional<std::string> description = {};

		std::shared_ptr<utils::json_value> json() const
		{
			auto json = utils::json_value::create<utils::json_value_kind::OBJECT>();
			if (detail.has_value()) {json->obj_add_str("detail", detail.value());}
			if (description.has_value()) {json->obj_add_str("description", description.value());}
			return json;
		}
	};

	enum class completion_item_kind: int {
		TEXT = 1,
		METHOD = 2,
		FUNCTION = 3,
		CONSTRUCTOR = 4,
		FIELD = 5,
		VARIABLE = 6,
		CLASS = 7,
		INTERFACE = 8,
		MODULE = 9,
		PROPERTY = 10,
		UNIT = 11,
		VALUE = 12,
		ENUM = 13,
		KEYWORD = 14,
		SNIPPET = 15,
		COLOR = 16,
		FILE = 17,
		REFERENCE = 18,
		FOLDER = 19,
		ENUMMEMBER = 20,
		CONSTANT = 21,
		STRUCT = 22,
		EVENT = 23,
		OPERATOR = 24,
		TYPEPARAMETER = 25,
	};

	constexpr const char *completion_item_kind_str(completion_item_kind kind)
	{
		switch (kind) {
		case completion_item_kind::TEXT: return "TEXT";
		case completion_item_kind::METHOD: return "METHOD";
		case completion_item_kind::FUNCTION: return "FUNCTION";
		case completion_item_kind::CONSTRUCTOR: return "CONSTRUCTOR";
		case completion_item_kind::FIELD: return "FIELD";
		case completion_item_kind::VARIABLE: return "VARIABLE";
		case completion_item_kind::CLASS: return "CLASS";
		case completion_item_kind::INTERFACE: return "INTERFACE";
		case completion_item_kind::MODULE: return "MODULE";
		case completion_item_kind::PROPERTY: return "PROPERTY";
		case completion_item_kind::UNIT: return "UNIT";
		case completion_item_kind::VALUE: return "VALUE";
		case completion_item_kind::ENUM: return "ENUM";
		case completion_item_kind::KEYWORD: return "KEYWORD";
		case completion_item_kind::SNIPPET: return "SNIPPET";
		case completion_item_kind::COLOR: return "COLOR";
		case completion_item_kind::FILE: return "FILE";
		case completion_item_kind::REFERENCE: return "REFERENCE";
		case completion_item_kind::FOLDER: return "FOLDER";
		case completion_item_kind::ENUMMEMBER: return "ENUMMEMBER";
		case completion_item_kind::CONSTANT: return "CONSTANT";
		case completion_item_kind::STRUCT: return "STRUCT";
		case completion_item_kind::EVENT: return "EVENT";
		case completion_item_kind::OPERATOR: return "OPERATOR";
		case completion_item_kind::TYPEPARAMETER: return "TYPEPARAMETER";
		default: throw internal_exception();
		}
	}

	enum class completion_item_tag : int {
		DEPRECATED = 1,
	};

	constexpr const char *completion_item_tag_str(completion_item_tag tag)
	{
		switch (tag) {
		case completion_item_tag::DEPRECATED: return "DEPRECATED";
		default: throw internal_exception();
		}
	}

	enum class insert_text_format: int {
		PLAIN_TEXT = 1,
		SNIPPET = 2,
	};

	constexpr const char *insert_text_format_str(insert_text_format fmt)
	{
		switch (fmt) {
		case insert_text_format::PLAIN_TEXT: return "PLAIN_TEXT";
		case insert_text_format::SNIPPET: return "SNIPPET";
		default: throw internal_exception();
		}
	}

	struct insert_replace_edit {
		std::string new_text;
		range insert;
		range replace;

		std::shared_ptr<utils::json_value> json() const
		{
			auto json = utils::json_value::create<utils::json_value_kind::OBJECT>();
			json->obj_add_str("newText", new_text);
			json->obj_add_obj("insert", insert.json());
			json->obj_add_obj("replace", replace.json());
			return json;
		}
	};

	enum class insert_text_mode: int {
		AS_IS = 1,
		ADJUST_INDENTATION = 2,
	};

	constexpr const char *insert_text_mode_str(insert_text_mode mode)
	{
		switch (mode) {
		case insert_text_mode::AS_IS: return "AS_IS";
		case insert_text_mode::ADJUST_INDENTATION: return "ADJUST_INDENTATION";
		default: throw internal_exception();
		}
	}

	struct completion_item {
		std::string label;
		std::optional<completion_item_label_details> label_details = {};
		std::optional<completion_item_kind> kind = {};
		std::optional<std::vector<completion_item_tag>> tags = {};
		std::optional<std::string> detail = {};
		std::optional<std::variant<std::string, markup_content>> documentation = {};
		std::optional<bool> preselect = {};
		std::optional<std::string> sort_text = {};
		std::optional<std::string> filter_text = {};
		std::optional<std::string> insert_text = {};
		std::optional<models::insert_text_format> insert_text_format = {};
		std::optional<models::insert_text_mode> insert_text_mode = {};
		std::optional<std::variant<models::text_edit, insert_replace_edit>> text_edit = {};
		std::optional<std::string> text_edit_text = {};
		std::optional<std::vector<models::text_edit>> additional_text_edits = {};
		std::optional<std::vector<std::string>> commit_characters = {};
		std::optional<models::command> command = {};
		std::optional<std::shared_ptr<utils::json_value>> data = {};

		std::shared_ptr<utils::json_value> json() const
		{
			auto json = utils::json_value::create<utils::json_value_kind::OBJECT>();

			json->obj_add_str("label", label);
			if (label_details.has_value()) {json->obj_add_obj("labelDetails", label_details.value().json());}
			json->obj_add_str("label", label);
			if (kind.has_value()) {json->obj_add_num("kind", (double)kind.value());}
			if (tags.has_value()) {
				auto arr = json->obj_add_arr("tags").value();
				for (auto tag : tags.value()) {
					arr->arr_add_num((double)tag);
				}
			}
			if (detail.has_value()) {json->obj_add_str("detail", detail.value());}
			if (documentation.has_value()) {
				auto &doc = documentation.value();
				if (std::holds_alternative<std::string>(doc)) {
					json->obj_add_str("documentation", std::get<std::string>(doc));
				} else {
					json->obj_add_obj("documentation", std::get<markup_content>(doc).json());
				}
			}
			if (preselect.has_value()) {json->obj_add_bool("preselect", preselect.value());}
			if (sort_text.has_value()) {json->obj_add_str("sortText", sort_text.value());}
			if (filter_text.has_value()) {json->obj_add_str("filterText", filter_text.value());}
			if (insert_text.has_value()) {json->obj_add_str("insertText", insert_text.value());}
			if (insert_text_format.has_value()) {json->obj_add_num("InsertTextFormat", (double)insert_text_format.value());}
			if (insert_text_mode.has_value()) {json->obj_add_num("insertTextMode", (double)insert_text_mode.value());}
			if (text_edit.has_value()) {
				if (std::holds_alternative<models::text_edit>(text_edit.value())) {
					json->obj_add_obj("textEdit", std::get<models::text_edit>(text_edit.value()).json());
				} else {
					json->obj_add_obj("textEdit", std::get<insert_replace_edit>(text_edit.value()).json());
				}
			}
			if (text_edit_text.has_value()) {json->obj_add_str("textEditText", text_edit_text.value());}
			if (additional_text_edits.has_value()) {
				auto arr = json->obj_add_arr("additionalTextEdits").value();
				for (const auto &edit : additional_text_edits.value()) {
					arr->arr_add_obj(edit.json());
				}
			}
			if (commit_characters.has_value()) {
				auto arr = json->obj_add_arr("commitCharacters").value();
				for (const auto &chars : commit_characters.value()) {
					arr->arr_add_str(chars);
				}
			}
			if (command.has_value()) {
				json->obj_add_obj("command", command.value().json());
			}

			if (data.has_value()) {json->obj_add_obj("data", data.value());}

			return json;
		}
	};

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

			auto items_arr = json->obj_add_arr("items").value();
			for (const auto &item : items) {
				items_arr->arr_add_obj(item.json());
			}

			return json;
		}
	};

	/*struct text_document_content_change_event {
#if 0
		range: Range;
#endif
		std::optional<uint32_t> range_length = {}; // rangeLength
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
