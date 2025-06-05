#ifndef HILL__LSP__ROUTER_HPP_INCLUDED
#define HILL__LSP__ROUTER_HPP_INCLUDED

#include "request.hpp"
#include "../utils/json_parser.hpp"

#include <unordered_map>
#include <functional>
#include <optional>
#include <memory>

#include "server_state.hpp"

namespace hill::lsp {

	std::optional<int> initialize(std::optional<std::shared_ptr<::hill::utils::json_value>> params)
	{
		auto &state = server_state::get();
		state.initialized = true;
		return 1;
	}

	struct router {
		router() = default;

		typedef std::function<std::optional<int>(std::optional<std::shared_ptr<::hill::utils::json_value>> params)> endpoint_t;

		const std::unordered_map<method, endpoint_t> map = {
			{method::INITIALIZE, initialize}
		};

		std::optional<endpoint_t> get(method m)
		{
			if (map.contains(m)) {
				return map.at(m);
			} else {
				return std::nullopt;
			}
		}
	};
}

#endif /* HILL__LSP__ROUTER_HPP_INCLUDED */
