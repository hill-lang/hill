#ifndef HILL__LSP__METHODS_INITIALIZED_HPP_INCLUDED
#define HILL__LSP__METHODS_INITIALIZED_HPP_INCLUDED

#include "../../utils/json_parser.hpp"
#include "../server_state.hpp"

#include <memory>
#include <optional>

namespace hill::lsp::methods {

	// Awknownlage initialize success
	inline std::optional<int> initialized(std::optional<std::shared_ptr<::hill::utils::json_value>> params)
	{
		auto &state = server_state::get();
		state.initialized = true;
		state.log.info("Initialized");
		return 1;
	}
};

#endif /* HILL__LSP__METHODS_INITIALIZED_HPP_INCLUDED */
