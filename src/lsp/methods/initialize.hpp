#ifndef HILL__LSP__METHODS_INITIALIZE_HPP_INCLUDED
#define HILL__LSP__METHODS_INITIALIZE_HPP_INCLUDED

#include "../models.hpp"
#include "../server_state.hpp"

#include <memory>
#include <optional>
#include <variant>

namespace hill::lsp::methods {

	// Requset to initialize
	inline std::variant<models::result_t, models::response_error> initialize(const models::request_message &req)
	{
		models::initialize_result result = {
			.capabilities = {
				.completion_provider = models::completion_options{}
			},
			.server_info = models::server_info{
				.name = "hill-lsp",
				.version = "0.0.1",
			}
		};

		return result.json();
	}

	// Awknownlage initialize success
	inline void initialized()
	{
		auto &state = server_state::get();
		state.initialized = true;
		state.log.info("Initialized");
	}
};

#endif /* HILL__LSP__METHODS_INITIALIZE_HPP_INCLUDED */
