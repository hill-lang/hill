#ifndef HILL__LSP__REQUEST_HANDLER_HPP_INCLUDED
#define HILL__LSP__REQUEST_HANDLER_HPP_INCLUDED

#include "models.hpp"
#include "router.hpp"
#include "../utils/json.hpp"

#include <memory>
#include <stdio.h>
#include <mutex>

namespace hill::lsp {

	struct request_handler {
		request_handler() = delete;

		static void handle(const std::shared_ptr<utils::json_value> &json)
		{
			using namespace ::hill::utils;

			auto &state = server_state::get();

			if (json->kind != json_value_kind::OBJECT) return;
			if (!json->object_contains("jsonrpc") || !json->object_contains("method")) {
				state.log.error("Missing required json fields in object - " + json->to_str());
				return;
			}

			auto method_json = json->object("method");
			if (!method_json.has_value() || method_json.value()->kind!=json_value_kind::STRING) return;
			auto method_opt = models::method_parse(method_json.value()->string);
			if (!method_opt.has_value()) {
				state.log.error("Unknown method [" + method_json.value()->string + "]");
				return;
			}
			auto method = method_opt.value();

			if (json->object_contains("id")) {
				handle_request(method, json);
			} else {
				handle_notification(method, json);
			}
		}

	private:
		static std::mutex &get_response_mutex()
		{
			static std::mutex mutex;
			return mutex;
		}

		static void handle_notification(models::method method, const std::shared_ptr<utils::json_value> &json)
		{
			auto &state = server_state::get();

			auto method_str = std::string(models::method_str(method));

			state.log.info("Received notification method<" + method_str + ">");
			auto func = router::get_notify(method);
			if (!func.has_value()) {
				state.log.error("Fail to resolve notification method " + method_str);
				return;
			}

			func.value()();
		}

		static inline void handle_request(models::method method, const std::shared_ptr<utils::json_value> &json)
		{
			using namespace ::hill::utils;

			auto &state = server_state::get();

			auto id_json = json->object("id");
			if (!id_json.has_value() || id_json.value()->kind!=json_value_kind::NUMBER) {
				state.log.error("Invalid request id");
				return;
			};

			auto id = (int)id_json.value()->number;
			auto method_str = std::string(models::method_str(method));

			state.log.info("Received request method<" + method_str + "> id<" + std::to_string(id) + ">");

			models::request_message req = {
				.id = id,
				.method = method,
				.params = json->object("params")};

			auto func = router::get_req(method);
			if (!func.has_value()) {
				state.log.error("Fail to resolve request method " + method_str);
				return;
			}

			auto result = func.value()(req);
			models::response_message resp_msg = {.id=id, .result=std::nullopt, .error=std::nullopt};
			if (std::holds_alternative<models::result_t>(result)) {
				resp_msg.result = std::get<models::result_t>(result);
			} else {
				resp_msg.error = std::get<models::response_error>(result);
				state.log.error("Request failed method<" + method_str
					+ "> id<" + std::to_string(id)
					+ "> code< "+ std::to_string((int)resp_msg.error.value().code)
					+ "> message<" + resp_msg.error.value().message + '>');
			}

			auto resp_msg_str = resp_msg.str();

			std::string resp_header = "Content-Length: "
				+ std::to_string(resp_msg_str.size())
				+ "\r\n\r\n";
			std::stringstream ss;
			ss.write(resp_header.c_str(), resp_header.size());
			ss.write(resp_msg_str.c_str(), resp_msg_str.size());

			auto resp = ss.str();

			{
				std::lock_guard<std::mutex> guard(get_response_mutex());

				fwrite(resp.c_str(), 1, resp.size(), stdout);
				fflush(stdout);
			}

			state.log.info("Sent response to request method<" + method_str + "> id<" + std::to_string(id) + ">");
		}
	};
};

#endif /* HILL__LSP__REQUEST_HANDLER_HPP_INCLUDED */
