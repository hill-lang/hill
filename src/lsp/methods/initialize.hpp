#ifndef HILL__LSP__METHODS_INITIALIZE_HPP_INCLUDED
#define HILL__LSP__METHODS_INITIALIZE_HPP_INCLUDED

#include "../../utils/json_parser.hpp"
#include "../server_state.hpp"

#include <memory>
#include <optional>

namespace hill::lsp::methods {

	// Requset to initialize
	inline std::optional<int> initialize(const models::request_message &req)
	{
		return 1;
	}

	// Awknownlage initialize success
	inline void initialized(std::optional<std::shared_ptr<::hill::utils::json_value>> params)
	{
		auto &state = server_state::get();
		state.initialized = true;
		state.log.info("Initialized");
	}
};

#endif /* HILL__LSP__METHODS_INITIALIZE_HPP_INCLUDED */
