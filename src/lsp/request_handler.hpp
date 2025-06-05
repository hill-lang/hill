#ifndef HILL__LSP__REQUEST_HANDLER_HPP_INCLUDED
#define HILL__LSP__REQUEST_HANDLER_HPP_INCLUDED

#include "request.hpp"
#include "router.hpp"
#include "response_builder.hpp"

#include <memory>
#include <stdio.h>

namespace hill::lsp::request_handler {

	static std::string foo(size_t id)
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

		return lsp::response_builder::build(id, result);
	}

	static void handle(std::shared_ptr<request> req)
	{
		auto &state = server_state::get();

		auto metstr = std::string(method_str(req->metod));

		if (req->is_notification()) {
			state.log.info("Received notification method [" + metstr + "]");
		} else {
			state.log.info("Received method [" + metstr + "] id: [" + std::to_string(req->id.value()) + "] ");
		}

		auto func = router::get(req->metod);
		if (!func.has_value()) {
			state.log.error("Fail to resolve method " + metstr);
			return;
		}

		auto result = func.value()(req->params);
		if (!result.has_value()) return;

		// We only have to reply to messages with an id
		if (req->is_notification()) return;

		//auto resp = res_builder.build(req->id, result);
		auto resp = foo(req->id.value());
		state.log.trace(resp);

		fwrite(resp.c_str(), 1, resp.size(), stdout);
		fflush(stdout);
	}
};

#endif /* HILL__LSP__REQUEST_HANDLER_HPP_INCLUDED */
