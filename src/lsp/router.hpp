#ifndef HILL__LSP__ROUTER_HPP_INCLUDED
#define HILL__LSP__ROUTER_HPP_INCLUDED

#include "models.hpp"

#include "methods/initialize.hpp"
#include "methods/text_document/completion.hpp"

#include <unordered_map>
#include <functional>
#include <optional>
#include <memory>
#include <variant>

#include "server_state.hpp"

namespace hill::lsp {

	struct router {
		router() = delete;

		typedef std::function<std::variant<models::result_t, models::response_error>(const models::request_message &req)> request_endpoint_t;
		typedef std::function<void()> notify_endpoint_t;

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
				{models::method::TEXT_DOCUMENT_COMPLETION, methods::text_document_completion},
			};
			return map;
		}

		static const std::unordered_map<models::method, notify_endpoint_t> &get_notify_map()
		{
			static const std::unordered_map<models::method, notify_endpoint_t> map = {
				{models::method::INITIALIZED, methods::initialized},
			};
			return map;
		}
	};
}

#endif /* HILL__LSP__ROUTER_HPP_INCLUDED */
