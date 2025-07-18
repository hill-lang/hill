#ifndef HILL__LSP__METHODS_LIFECYCLE_HH_INCLUDED
#define HILL__LSP__METHODS_LIFECYCLE_HH_INCLUDED

#include "../models.hh"
#include "../logger.hh"
#include "../server_state.hh"

namespace hill::lsp::methods {

	/**
	 * Requset to initialize
	 */
	inline std::variant<std::optional<models::result_t>, models::response_error> initialize(const models::request_message &req)
	{
		(void)req; // Unused
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

	/**
	 * Awknownlage initialize success
	 */
	inline void initialized(const models::notification_message &notify)
	{
		(void)notify; // Unused
		auto &state = server_state::get();
		state.initialized = true;
		logger::info("Initialized");
	}

	/**
	 * A notification that should be used by the client to modify the trace setting of the server
	 */
	inline void set_trace(const models::notification_message &notify)
	{
		(void)notify; // Unused
		// TODO: Implement
		logger::info("Set trace ??");
	}

	/**
	 * The server should shut down, but not exit
	 */
	inline std::variant<std::optional<models::result_t>, models::response_error> shutdown(const models::request_message &req)
	{
		(void)req; // Unused
		auto &state = server_state::get();
		state.initialized = false;
		logger::info("Shutdown");
		return {};
	}

	/**
	 * The server should exit
	 */
	inline void exit(const models::notification_message &notify)
	{
		(void)notify; // Unused
		auto &state = server_state::get();
		state.running = false;
		logger::info("Exit");
	}
};

#endif /* HILL__LSP__METHODS_LIFECYCLE_HH_INCLUDED */
