#ifndef HILL__LSP__METHODS_TEMPLATE_HPP_INCLUDED
#define HILL__LSP__METHODS_TEMPLATE_HPP_INCLUDED

#include "../../utils/json_parser.hpp"

#include <memory>
#include <optional>

namespace hill::lsp::methods {

	inline std::optional<int> _template(std::optional<std::shared_ptr<::hill::utils::json_value>> params)
	{
		return 1;
	}
};

#endif /* HILL__LSP__METHODS_TEMPLATE_HPP_INCLUDED */
