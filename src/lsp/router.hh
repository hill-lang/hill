#ifndef HILL__LSP__ROUTER_HH_INCLUDED
#define HILL__LSP__ROUTER_HH_INCLUDED

#include "models.hh"

#include "methods/cancel_request.hh"
#include "methods/lifecycle.hh"
#include "methods/text_document/completion.hh"
#include "methods/text_document/formatting.hh"
#include "methods/text_document/hover.hh"
#include "methods/text_document/synchronization.hh"

#include <unordered_map>
#include <functional>
#include <optional>
#include <memory>
#include <variant>

namespace hill::lsp {

	struct router {
		router() = delete;

		typedef std::variant<std::optional<models::result_t>, models::response_error> request_result_t;
		typedef std::function<request_result_t(const models::request_message &req)> request_endpoint_t;

		typedef std::function<void(const models::notification_message &req)> notify_endpoint_t;

		static std::optional<request_endpoint_t> get_req(models::method m)
		{
			auto &map = get_request_map();
			if (map.contains(m)) {
				return map.at(m);
			} else {
				return {};
			}
		}

		static std::optional<notify_endpoint_t> get_notify(models::method m)
		{
			auto &map = get_notify_map();
			if (map.contains(m)) {
				return map.at(m);
			} else {
				return {};
			}
		}

	private:
		static const std::unordered_map<models::method, request_endpoint_t> &get_request_map()
		{
			static const std::unordered_map<models::method, request_endpoint_t> map = {
				{models::method::INITIALIZE, methods::initialize},
				{models::method::SHUTDOWN, methods::shutdown},
				{models::method::TEXT_DOCUMENT_COMPLETION, methods::text_document_completion},
				{models::method::TEXT_DOCUMENT_HOVER, methods::text_document_hover},
				{models::method::TEXT_DOCUMENT_FORMATTING, methods::text_document_formatting},
			};
			return map;
		}

		static const std::unordered_map<models::method, notify_endpoint_t> &get_notify_map()
		{
			static const std::unordered_map<models::method, notify_endpoint_t> map = {
				{models::method::CANCEL_REQUEST, methods::cancel_request},
				{models::method::INITIALIZED, methods::initialized},
				{models::method::SET_TRACE, methods::set_trace},
				{models::method::EXIT, methods::exit},
				{models::method::TEXT_DOCUMENT_DID_OPEN, methods::text_document_did_open},
				{models::method::TEXT_DOCUMENT_DID_CHANGE, methods::text_document_did_change},
				{models::method::TEXT_DOCUMENT_DID_CLOSE, methods::text_document_did_close},
			};
			return map;
		}
	};
}

#endif /* HILL__LSP__ROUTER_HH_INCLUDED */
