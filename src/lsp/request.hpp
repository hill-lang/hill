#ifndef HILL__LSP__REQUEST_HPP_INCLUDED
#define HILL__LSP__REQUEST_HPP_INCLUDED

#include "../exceptions.hpp"
#include "../utils/json_parser.hpp"

#include <stddef.h>
#include <string>
#include <optional>

namespace hill::lsp {

	enum class method {
		INITIALIZE, // Requset to initialize
		INITIALIZED, // Awknownlage initialize success
	};

	constexpr const char *method_str(method m)
	{
		switch (m) {
		case method::INITIALIZE: return "initialize";
		case method::INITIALIZED: return "initialized";
		default: throw internal_exception();
		}
	}

	constexpr std::optional<method> method_parse(const std::string &str)
	{
		if (str == method_str(method::INITIALIZE)) return method::INITIALIZE;
		if (str==method_str(method::INITIALIZED)) return method::INITIALIZED;
		else return std::nullopt;
	}

	struct request {
		request(method metod): id(std::nullopt), metod(metod), params(std::nullopt) {}
		request(method metod, size_t id, const std::shared_ptr<::hill::utils::json_value> &params): metod(metod), id(id), params(params) {}

		method metod;
		// Request id, should be echoed in the response
		// If there is no id, this request is a notification
		std::optional<size_t> id;
		std::optional<std::shared_ptr<::hill::utils::json_value>> params;

		bool is_notification() const {return !id.has_value();}
	};
}

#endif /* HILL__LSP__REQUEST_HPP_INCLUDED */
