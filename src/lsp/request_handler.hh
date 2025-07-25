#ifndef HILL__LSP__REQUEST_HANDLER_HH_INCLUDED
#define HILL__LSP__REQUEST_HANDLER_HH_INCLUDED

#include "models.hh"
#include "router.hh"
#include "logger.hh"
#include "../utils/json.hh"

#include <memory>
#include <stdio.h>
#include <mutex>

namespace hill::lsp {

	struct request_handler {
		request_handler() = delete;

		static void handle(const std::shared_ptr<utils::json_value> &json)
		{
			using namespace ::hill::utils;

			if (json->kind()!=json_value_kind::OBJECT) return;
			if (!json->obj_has("jsonrpc") || !json->obj_has("method")) {
				logger::error("Missing required json fields in object - " + json->stringify());
				return;
			}

			auto method_json = json->obj_get("method");
			if (!method_json || (*method_json)->kind()!=json_value_kind::STRING) return;
			auto method_opt = models::method_parse(*(*method_json)->str());
			if (!method_opt) {
				logger::error("Unknown method [" + *(*method_json)->str() + "]");
				return;
			}
			auto method = *method_opt;

			if (json->obj_has("id")) {
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
			auto method_str = std::string(models::method_str(method));

			logger::info("Received notify method<" + method_str + ">");

			models::notification_message notification = {
				.method = method,
				.params = json->obj_get("params")};

			auto func = router::get_notify(method);
			if (!func) {
				logger::error("Fail to resolve notify method " + method_str);
				return;
			}

			(*func)(notification);
		}

		static inline void handle_request(models::method method, const std::shared_ptr<utils::json_value> &json)
		{
			using namespace ::hill::utils;

			auto id_json = json->obj_get("id");
			if (!id_json || (*id_json)->kind()!=json_value_kind::NUMBER) {
				logger::error("Invalid request id");
				return;
			};

			auto id = (int)*(*id_json)->num();
			auto method_str = std::string(models::method_str(method));

			logger::info("Received method<" + method_str + "> id<" + std::to_string(id) + ">");
			server_state::get().request_state.on_recv(id);

			models::request_message req = {
				.id = id,
				.method = method,
				.params = json->obj_get("params")};

			auto func = router::get_req(method);
			if (!func) {
				logger::error("Fail to resolve request method " + method_str);
				return;
			}

			auto result = (*func)(req);
			models::response_message resp_msg = {.id=id};

			if (server_state::get().request_state.cancelled(id)) {
				resp_msg.error = models::response_error{
					.code = models::error_code::REQUEST_CANCELLED,
					.message = "Request cancelled"};
				logger::info("Request cancelled - method<" + method_str
					+ "> id<" + std::to_string(id)
					+ "> code< " + std::to_string((int)(*resp_msg.error).code)
					+ "> message<" + (*resp_msg.error).message + '>');
			} else {
				if (std::holds_alternative<std::optional<models::result_t>>(result)) {
					resp_msg.result = std::get<std::optional<models::result_t>>(result);
				} else {
					resp_msg.error = std::get<models::response_error>(result);
					logger::info("Request error - method<" + method_str
						+ "> id<" + std::to_string(id)
						+ "> code< " + std::to_string((int)(*resp_msg.error).code)
						+ "> message<" + (*resp_msg.error).message + '>');
				}
			}

			auto resp_msg_str = resp_msg.json()->stringify();

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

			logger::info("Sent response method<" + method_str + "> id<" + std::to_string(id) + ">");
			server_state::get().request_state.on_complete(id);
		}
	};
};

#endif /* HILL__LSP__REQUEST_HANDLER_HH_INCLUDED */
