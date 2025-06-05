#ifndef HILL__UTILS__JSON_HPP_INCLUDED
#define HILL__UTILS__JSON_HPP_INCLUDED

#include <exception>
#include <memory>
#include <optional>
#include <string>
#include <sstream>
#include <vector>
#include <variant>

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
		json_value(const std::string &string): kind(json_value_kind::STRING), string(string) {}
		json_value(double number): kind(json_value_kind::NUMBER), number(number) {}
		json_value(bool boolean): kind(json_value_kind::BOOL), boolean(boolean) {}

		json_value_kind kind;

		//std::variant<> // TODO: Use variant
		std::vector<std::pair<std::string, std::shared_ptr<json_value>>> object_entries;
		std::vector<std::shared_ptr<json_value>> array;
		std::string string;
		double number = 0.0;
		bool boolean = false;

		static std::shared_ptr<json_value> create(json_value_kind kind) {return std::make_shared<json_value>(kind);}
		static std::shared_ptr<json_value> create(const std::string &string) {return std::make_shared<json_value>(string);}
		static std::shared_ptr<json_value> create(double number) {return std::make_shared<json_value>(number);}
		static std::shared_ptr<json_value> create(bool boolean) {return std::make_shared<json_value>(boolean);}

		/// <summary>
		/// Add object to object
		/// </summary>
		std::optional<std::shared_ptr<json_value>> obj_add_obj(const std::string &key)
		{
			if (kind!=json_value_kind::OBJECT) return std::nullopt;
			if (object_contains(key)) return std::nullopt;

			auto val = create(json_value_kind::OBJECT);
			object_entries.emplace_back(key, val);
			return val;
		}

		/// <summary>
		/// Add array to object
		/// </summary>
		std::optional<std::shared_ptr<json_value>> obj_add_arr(const std::string &key)
		{
			if (kind!=json_value_kind::OBJECT) return std::nullopt;
			if (object_contains(key)) return std::nullopt;

			auto val = create(json_value_kind::ARRAY);
			object_entries.emplace_back(key, val);
			return val;
		}

		/// <summary>
		/// Add string to object
		/// </summary>
		std::optional<std::shared_ptr<json_value>> obj_add_str(const std::string &key, const std::string &string)
		{
			if (kind!=json_value_kind::OBJECT) return std::nullopt;
			if (object_contains(key)) return std::nullopt;

			auto val = create(string);
			object_entries.emplace_back(key, val);
			return val;
		}

		/// <summary>
		/// Add number to object
		/// </summary>
		std::optional<std::shared_ptr<json_value>> obj_add_num(const std::string &key, double number)
		{
			if (kind!=json_value_kind::OBJECT) return std::nullopt;
			if (object_contains(key)) return std::nullopt;

			auto val = create(number);
			object_entries.emplace_back(key, val);
			return val;
		}

		/// <summary>
		/// Add boolean to object
		/// </summary>
		std::optional<std::shared_ptr<json_value>> obj_add_bool(const std::string &key, bool boolean)
		{
			if (kind!=json_value_kind::OBJECT) return std::nullopt;
			if (object_contains(key)) return std::nullopt;

			auto val = create(boolean);
			object_entries.emplace_back(key, val);
			return val;
		}

		/// <summary>
		/// Add object to array
		/// </summary>
		std::optional<std::shared_ptr<json_value>> arr_add_obj()
		{
			if (kind!=json_value_kind::ARRAY) return std::nullopt;

			auto obj = create(json_value_kind::OBJECT);
			array.push_back(obj);
		}

		/// <summary>
		/// Add array to array
		/// </summary>
		std::optional<std::shared_ptr<json_value>> arr_add_arr()
		{
			if (kind!=json_value_kind::ARRAY) return std::nullopt;

			auto val = create(json_value_kind::ARRAY);
			array.push_back(val);
			return val;
		}

		/// <summary>
		/// Add string to array
		/// </summary>
		std::optional<std::shared_ptr<json_value>> arr_add_str(const std::string &string)
		{
			if (kind!=json_value_kind::ARRAY) return std::nullopt;

			auto val = create(string);
			array.push_back(val);
			return val;
		}

		/// <summary>
		/// Add number to array
		/// </summary>
		std::optional<std::shared_ptr<json_value>> obj_add_arr(double number)
		{
			if (kind!=json_value_kind::ARRAY) return std::nullopt;

			auto val = create(number);
			array.push_back(val);
			return val;
		}

		/// <summary>
		/// Add boolean to array
		/// </summary>
		std::optional<std::shared_ptr<json_value>> obj_add_bool(bool boolean)
		{
			if (kind!=json_value_kind::ARRAY) return std::nullopt;

			auto val = create(boolean);
			array.push_back(val);
			return val;
		}

		/// <summary>
		/// Get a member from an object
		/// </summary>
		std::optional<std::shared_ptr<json_value>> obj_get(const std::string &key)
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
