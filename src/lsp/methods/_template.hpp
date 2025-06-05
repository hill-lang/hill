#ifndef HILL__LSP__METHODS_TEMPLATE_HPP_INCLUDED
#define HILL__LSP__METHODS_TEMPLATE_HPP_INCLUDED

#include "../models.hpp"

#include <memory>
#include <optional>
#include <variant>

namespace hill::lsp::methods {

	inline std::variant<models::result_t, models::response_error> _request_template(const models::request_message &req)
	{
		throw;
	}

	inline void _notify_template()
	{
		throw;
	}
};

#endif /* HILL__LSP__METHODS_TEMPLATE_HPP_INCLUDED */
