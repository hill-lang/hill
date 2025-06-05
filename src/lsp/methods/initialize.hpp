#ifndef HILL__LSP__METHODS_INITIALIZE_HPP_INCLUDED
#define HILL__LSP__METHODS_INITIALIZE_HPP_INCLUDED

#include "../models.hpp"
#include "../server_state.hpp"
#include "../../utils/json.hpp"

#include <memory>
#include <optional>
#include <variant>

namespace hill::lsp::methods {

	// Requset to initialize
	inline std::variant<models::result_t, models::response_error> initialize(const models::request_message &req)
	{
		auto json = utils::json_value::create(utils::json_value_kind::OBJECT);

		auto capabilities = json->obj_add_obj("capabilities").value();
		auto completion_provider = json->obj_add_obj("completionProvider").value();

		auto server_info = json->obj_add_obj("serverInfo").value();
		server_info->obj_add_str("name", "hill-lsp");
		server_info->obj_add_str("version", "0.0.1");

		return json;
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
