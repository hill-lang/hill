#ifndef HILL__LSP__ROUTER_HPP_INCLUDED
#define HILL__LSP__ROUTER_HPP_INCLUDED

#include "request.hpp"
#include <functional>

namespace hill::lsp {

	struct router {

		std::function<int()> get(method m)
		{
			return nullptr;
		}
	};
}

#endif /* HILL__LSP__ROUTER_HPP_INCLUDED */
