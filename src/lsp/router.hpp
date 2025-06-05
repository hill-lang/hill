#ifndef HILL__LSP__ROUTER_HPP_INCLUDED
#define HILL__LSP__ROUTER_HPP_INCLUDED

#include "request.hpp"

#include <functional>
#include <optional>

namespace hill::lsp {

	struct router {

		std::optional<std::function<int()>> get(method m)
		{
			return nullptr;
		}
	};
}

#endif /* HILL__LSP__ROUTER_HPP_INCLUDED */
