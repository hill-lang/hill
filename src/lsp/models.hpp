#ifndef HILL__LSP__MODELS_HPP_INCLUDED
#define HILL__LSP__MODELS_HPP_INCLUDED

#include "../exceptions.hpp"
#include "../utils/json.hpp"

#include <optional>
#include <memory>
#include <string>

namespace hill::lsp::models {

	enum class method {
		INITIALIZE, // Requset to initialize
		INITIALIZED, // Awknownlage initialize success
		TEXT_DOCUMENT_COMPLETION,
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

		std::shared_ptr<utils::json_value> json()
		{
			auto json = utils::json_value::create(utils::json_value_kind::OBJECT);

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

		std::shared_ptr<utils::json_value> json()
		{
			auto json = utils::json_value::create(utils::json_value_kind::OBJECT);

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

	struct completion_client_capabilities {
		/**
		 * Whether completion supports dynamic registration.
		 */
		//std::optional<bool> dynamic_registration;
	};
};

#endif /* HILL__LSP__MODELS_HPP_INCLUDED */
