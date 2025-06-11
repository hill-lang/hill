#ifndef HILL__LSP__ROUTER_HPP_INCLUDED
#define HILL__LSP__ROUTER_HPP_INCLUDED

#include "models.hpp"

#include "methods/lifecycle.hpp"
#include "methods/text_document/completion.hpp"
#include "methods/text_document/hover.hpp"
#include "methods/text_document/synchronization.hpp"

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
				return std::nullopt;
			}
		}

		static std::optional<notify_endpoint_t> get_notify(models::method m)
		{
			auto &map = get_notify_map();
			if (map.contains(m)) {
				return map.at(m);
			} else {
				return std::nullopt;
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
			};
			return map;
		}

		static const std::unordered_map<models::method, notify_endpoint_t> &get_notify_map()
		{
			static const std::unordered_map<models::method, notify_endpoint_t> map = {
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

#endif /* HILL__LSP__ROUTER_HPP_INCLUDED */
