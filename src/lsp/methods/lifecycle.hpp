#ifndef HILL__LSP__METHODS_LIFECYCLE_HPP_INCLUDED
#define HILL__LSP__METHODS_LIFECYCLE_HPP_INCLUDED

#include "../models.hpp"
#include "../logger.hpp"
#include "../server_state.hpp"

namespace hill::lsp::methods {

	// Requset to initialize
	inline std::variant<std::optional<models::result_t>, models::response_error> initialize(const models::request_message &req)
	{
		auto &state = server_state::get();
		models::initialize_result result = {
			.capabilities = state.server_capabilities,
			.server_info = models::server_info{
				.name = "hill-lsp",
				.version = "0.0.1",
			}
		};

		return result.json();
	}

	// Awknownlage initialize success
	inline void initialized(const models::notification_message &notify)
	{
		auto &state = server_state::get();
		state.initialized = true;
		logger::info("Initialized");
	}

	// The server should shut down, but not exit
	inline std::variant<std::optional<models::result_t>, models::response_error> shutdown(const models::request_message &req)
	{
		auto &state = server_state::get();
		state.initialized = false;
		logger::info("Shutdown");
		return std::nullopt;
	}

	// The server should shut down, but not exit
	inline void exit(const models::notification_message &notify)
	{
		auto &state = server_state::get();
		state.running = false;
		logger::info("Exit");
	}
};

#endif /* HILL__LSP__METHODS_LIFECYCLE_HPP_INCLUDED */
