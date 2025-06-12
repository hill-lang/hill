#ifndef HILL__LSP__METHODS_CANCEL_REQUEST_HPP_INCLUDED
#define HILL__LSP__METHODS_CANCEL_REQUEST_HPP_INCLUDED

#include "../models.hpp"
#include "../logger.hpp"
#include "../server_state.hpp"

namespace hill::lsp::methods {

	inline void cancel_request(const models::notification_message &notify)
	{
		auto params = models::cancel_params::from_json(notify.params.value()).value();
		logger::info("Received cancellation notification for request id<" + std::to_string(params.id) + ">");
		server_state::get().request_state.on_cancel(params.id);
	}
};

#endif /* HILL__LSP__METHODS_CANCEL_REQUEST_HPP_INCLUDED */
