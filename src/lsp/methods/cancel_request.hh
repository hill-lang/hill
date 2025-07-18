#ifndef HILL__LSP__METHODS_CANCEL_REQUEST_HH_INCLUDED
#define HILL__LSP__METHODS_CANCEL_REQUEST_HH_INCLUDED

#include "../models.hh"
#include "../logger.hh"
#include "../server_state.hh"

namespace hill::lsp::methods {

	inline void cancel_request(const models::notification_message &notify)
	{
		auto params = *models::cancel_params::from_json(*notify.params);
		logger::info("Received cancellation notification for request id<" + std::to_string(params.id) + ">");
		server_state::get().request_state.on_cancel(params.id);
	}
};

#endif /* HILL__LSP__METHODS_CANCEL_REQUEST_HH_INCLUDED */
