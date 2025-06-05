#ifndef HILL__LSP__SERVER_STATE_HPP_INCLUDED
#define HILL__LSP__SERVER_STATE_HPP_INCLUDED

#include "logger.hpp"

namespace hill::lsp {

	struct server_state {
		logger log;

		bool initialized = false;

		static server_state &get()
		{
			static server_state log;
			return log;
		}
	};
}

#endif /* HILL__LSP__SERVER_STATE_HPP_INCLUDED */
