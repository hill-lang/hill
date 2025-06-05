#ifndef HILL__LSP__REQUEST_HPP_INCLUDED
#define HILL__LSP__REQUEST_HPP_INCLUDED

#include "../utils/json_parser.hpp"

#include <stddef.h>
#include <string>
#include <optional>

namespace hill::lsp {

	enum class method {
		INITIALIZE,
	};

	constexpr std::optional<method> method_parse(const std::string &str)
	{
		if (str == "initialize") return method::INITIALIZE;
		else return std::nullopt;
	}

	struct request {
		request(method metod, const std::shared_ptr<::hill::utils::json_value> &params): id(std::nullopt), metod(metod), params(params) {}
		request(method metod, size_t id, const std::shared_ptr<::hill::utils::json_value> &params): metod(metod), id(id), params(params) {}

		method metod;
		std::optional<size_t> id; // Request id, should be echoed in the response
		std::shared_ptr<::hill::utils::json_value> params;
	};
}

#endif /* HILL__LSP__REQUEST_HPP_INCLUDED */
