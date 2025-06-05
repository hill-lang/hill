#ifndef HILL__LSP__ROUTER_HPP_INCLUDED
#define HILL__LSP__ROUTER_HPP_INCLUDED

#include "models.hpp"
#include "../utils/json_parser.hpp"

#include "methods/initialize.hpp"

#include <unordered_map>
#include <functional>
#include <optional>
#include <memory>

#include "server_state.hpp"

namespace hill::lsp {

	struct router {
		router() = default;

		typedef std::function<std::optional<int>(const models::request_message &req)> request_endpoint_t;
		typedef std::function<void(std::optional<std::shared_ptr<::hill::utils::json_value>> params)> notify_endpoint_t;

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
