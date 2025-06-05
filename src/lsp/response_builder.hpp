#ifndef HILL__LSP__RESPONSE_BUILDER_HPP_INCLUDED
#define HILL__LSP__RESPONSE_BUILDER_HPP_INCLUDED

#include "../utils/json_writer.hpp"

#include <filesystem>
#include <fstream>
#include <string>
#include <sstream>
#include <memory>

namespace hill::lsp::response_builder {

	inline std::string build(size_t id, const std::string &result)
	{
		auto oss = std::make_shared<std::ostringstream>();

		utils::json_writer json(oss);
		json.obj_i32("id", (int32_t)id);
		json.obj_raw("result", result);
		json.close();
		auto json_str = oss->str();

		std::string header = "Content-Length: "
			+ std::to_string(json_str.size())
			+ "\r\n\r\n";

		std::stringstream ss;
		ss.write(header.c_str(), header.size());
		ss.write(json_str.c_str(), json_str.size());
		return ss.str();
	}
}

#endif /* HILL__LSP__RESPONSE_BUILDER_HPP_INCLUDED */
