#ifndef HILL__UTILS__JSON_HPP_INCLUDED
#define HILL__UTILS__JSON_HPP_INCLUDED

#include <exception>
#include <memory>
#include <optional>
#include <string>
#include <sstream>
#include <vector>

namespace hill::utils {

	class json_internal_exception : public std::exception {
		const char *what() const noexcept override { return "JSON parser exception"; }
	};

	enum class json_value_kind {
		OBJECT,
		ARRAY,
		STRING,
		NUMBER,
		BOOL,
		JSON_NULL,
	};

	inline constexpr const char *json_value_kind_str(json_value_kind kind)
	{
		switch (kind) {
		case json_value_kind::OBJECT: return "OBJECT";
		case json_value_kind::ARRAY: return "ARRAY";
		case json_value_kind::STRING: return "STRING";
		case json_value_kind::NUMBER: return "NUMBER";
		case json_value_kind::BOOL: return "BOOL";
		case json_value_kind::JSON_NULL: return "NULL";
		default: throw json_internal_exception();
		}
	}

	struct json_value {
		json_value(json_value_kind kind): kind(kind) {}
		json_value(json_value_kind kind, const std::string &string): kind(kind), string(string) {}
		json_value(json_value_kind kind, double number): kind(kind), number(number) {}
		json_value(json_value_kind kind, bool boolean): kind(kind), boolean(boolean) {}

		json_value_kind kind;

		std::vector<std::pair<std::string, std::shared_ptr<json_value>>> object_entries;
		std::vector<std::shared_ptr<json_value>> array;
		std::string string;
		double number = 0.0;
		bool boolean = false;

		std::optional<std::shared_ptr<json_value>> object(const std::string &key)
		{
			auto it = std::find_if(object_entries.begin(), object_entries.end(),
				[&key](std::pair<std::string, std::shared_ptr<json_value>> pair) {
					return pair.first == key;
				});

			if (it != object_entries.end()) {
				auto ix = std::distance(object_entries.begin(), it);
				return object_entries[ix].second;
			} else {
				return std::nullopt;
			}
		}

		bool object_contains(const std::string &key) const
		{
			return std::find_if(object_entries.begin(), object_entries.end(),
				[&key](std::pair<std::string, std::shared_ptr<json_value>> pair) {
					return pair.first == key;
			}) != object_entries.end();
		}

		std::string to_str() const
		{
			std::stringstream ss;

			switch (kind) {
			case json_value_kind::OBJECT:
			{
				ss << '{';
				size_t ix=0;
				for (auto &[key, value] : object_entries) {
					if (ix++>0) ss << ',';
					ss << '"' << key << "\":" << value->to_str();
				}
				ss << '}';
				break;
			}
			case json_value_kind::ARRAY:
			{
				ss << '[';
				size_t ix=0;
				for (auto &value : array) {
					if (ix++>0) ss << ',';
					ss << value->to_str();
				}
				ss << ']';
				break;
			}
			case json_value_kind::STRING:
				ss << '"' << string << '"';
				break;
			case json_value_kind::NUMBER:
				ss << number;
				break;
			case json_value_kind::BOOL:
				ss << (boolean ? "true" : "false");
				break;
			case json_value_kind::JSON_NULL:
				ss << "null";
				break;
			default:
				throw json_internal_exception();
			}

			return ss.str();
		}

		std::string kind_str() const
		{
			std::stringstream ss;

			ss << json_value_kind_str(kind);

			switch (kind) {
			case json_value_kind::OBJECT:
			{
				if (object_entries.size()) {
					for (auto &[key, value] : object_entries) {
						ss << ',' << value->kind_str();
					}
					ss << ",END";
				}
				break;
			}
			case json_value_kind::ARRAY:
			{
				if (array.size()) {
					for (auto &value : array) {
						ss << ',' << value->kind_str();
					}
					ss << ",END";
				}
				break;
			}
			default:
				break;
			}

			return ss.str();
		}
	};
}

#endif /* HILL__UTILS__JSON_HPP_INCLUDED */
