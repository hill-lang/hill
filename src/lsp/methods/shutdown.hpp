#ifndef HILL__LSP__METHODS_SHUTDOWN_HPP_INCLUDED
#define HILL__LSP__METHODS_SHUTDOWN_HPP_INCLUDED

#include "../models.hpp"
#include "../logger.hpp"
#include "../server_state.hpp"

namespace hill::lsp::methods {

	inline std::variant<std::optional<models::result_t>, models::response_error> shutdown(const models::request_message &req)
	{
		auto &state = server_state::get();
		state.running = false;
		logger::info("Shutdown");
		return std::nullopt;
	}
};

#endif /* HILL__LSP__METHODS_SHUTDOWN_HPP_INCLUDED */
