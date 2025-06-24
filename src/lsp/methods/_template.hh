#ifndef HILL__LSP__METHODS_TEMPLATE_HH_INCLUDED
#define HILL__LSP__METHODS_TEMPLATE_HH_INCLUDED

#include "../models.hh"
#include "../logger.hh"

namespace hill::lsp::methods {

	inline std::variant<std::optional<models::result_t>, models::response_error> _request_template(const models::request_message &req)
	{
		throw;
	}

	inline void _notify_template(const models::notification_message &notify)
	{
		throw;
	}
};

#endif /* HILL__LSP__METHODS_TEMPLATE_HH_INCLUDED */
