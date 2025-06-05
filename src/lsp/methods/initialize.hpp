#ifndef HILL__LSP__METHODS_INITIALIZE_HPP_INCLUDED
#define HILL__LSP__METHODS_INITIALIZE_HPP_INCLUDED

#include "../models.hpp"
#include "../server_state.hpp"
#include "../../utils/json_parser.hpp"
#include "../../utils/json_writer.hpp"

#include <memory>
#include <optional>
#include <variant>

namespace hill::lsp::methods {

	// Requset to initialize
	inline std::variant<models::result_t, models::response_error> initialize(const models::request_message &req)
	{
		auto oss = std::make_shared<std::ostringstream>();
		utils::json_writer json(oss);
		json.obj("capabilities");
		json.obj("completionProvider");
		json.pop();
		json.pop();
		json.obj("serverInfo");
		json.obj_str("name", "hill-lsp");
		json.obj_str("version", "0.0.1");
		json.close();

		auto result = oss->str();
		std::istringstream istr(result);

		auto res = utils::json_parser::parse(istr);
		return res.value();
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
