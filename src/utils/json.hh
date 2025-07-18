#ifndef HILL__UTILS__JSON_HH_INCLUDED
#define HILL__UTILS__JSON_HH_INCLUDED

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

	inline std::string json_escape_str(const char *s)
	{
		std::stringstream ss;
		while (*s) {
			switch (*s) {
			case '\\': ss << "\\\\"; break;
			case '\n': ss << "\\n"; break;
			case '"': ss << "\\\""; break;
			default: ss << *s;
			}
			++s;
		}
		return ss.str();
	}

	struct json_value {
	private:
		friend struct json_parser;

		typedef std::vector<std::pair<std::string, std::shared_ptr<json_value>>> object_t;
		typedef std::vector<std::shared_ptr<json_value>> array_t;
		typedef double number_t;

		struct json_array_ref {
			const json_value::array_t &value;
			json_value::array_t::const_iterator begin() const { return value.cbegin(); }
			json_value::array_t::const_iterator end() const { return value.cend(); }
		};

		json_value_kind value_kind;
		std::variant<object_t, array_t, std::string, number_t, bool> value;

	public:
		json_value(json_value_kind value_kind): value_kind(value_kind)
		{
			if (value_kind==json_value_kind::OBJECT) {
				value = object_t();
			} else if (value_kind==json_value_kind::ARRAY) {
				value = array_t();
			}
		}
		json_value(const std::string &string): value_kind(json_value_kind::STRING), value(string) {}
		json_value(number_t number): value_kind(json_value_kind::NUMBER), value(number) {}
		json_value(bool boolean): value_kind(json_value_kind::BOOL), value(boolean) {}

		template<json_value_kind KIND> static std::shared_ptr<json_value> create()
		{
			static_assert(KIND==json_value_kind::OBJECT || KIND==json_value_kind::ARRAY || KIND == json_value_kind::JSON_NULL);
			return std::make_shared<json_value>(KIND);
		}
		static std::shared_ptr<json_value> create(const std::string &string) {return std::make_shared<json_value>(string);}
		static std::shared_ptr<json_value> create(number_t number) {return std::make_shared<json_value>(number);}
		static std::shared_ptr<json_value> create(bool boolean) {return std::make_shared<json_value>(boolean);}

		json_value_kind kind() const
		{
			return value_kind;
		}

		/*std::optional<const object_t &> obj() const
		{
			if (value_kind!=json_value_kind::OBJECT) return {};
			if (!std::holds_alternative<object_t>(value)) return {};
			return std::get<object_t>(value);
		}*/

		// NEVER extend the lifetine of this return value!
		std::optional<json_array_ref> arr() const
		{
			if (value_kind!=json_value_kind::ARRAY) return {};
			if (!std::holds_alternative<array_t>(value)) return {};
			return json_array_ref {std::get<array_t>(value)};
		}

		std::optional<std::string> str() const
		{
			if (value_kind!=json_value_kind::STRING) return {};
			if (!std::holds_alternative<std::string>(value)) return {};
			return std::get<std::string>(value);
		}

		std::optional<number_t> num() const
		{
			if (value_kind!=json_value_kind::NUMBER) return {};
			if (!std::holds_alternative<number_t>(value)) return {};
			return std::get<number_t>(value);
		}

		std::optional<bool> boolean() const
		{
			if (value_kind!=json_value_kind::BOOL) return {};
			if (!std::holds_alternative<bool>(value)) return {};
			return std::get<bool>(value);
		}

		/// <summary>
		/// Add object to object
		/// </summary>
		std::optional<std::shared_ptr<json_value>> obj_add_obj(const std::string &key)
		{
			if (value_kind!=json_value_kind::OBJECT) return {};
			if (obj_has(key)) return {};

			auto val = create<json_value_kind::OBJECT>();
			std::get<object_t>(value).emplace_back(key, val);
			return val;
		}

		/// <summary>
		/// Add existing object to object
		/// </summary>
		std::optional<std::shared_ptr<json_value>> obj_add_obj(const std::string &key, const std::shared_ptr<json_value> &obj)
		{
			if (value_kind!=json_value_kind::OBJECT) return {};
			if (obj_has(key)) return {};

			std::get<object_t>(value).emplace_back(key, obj);
			return obj;
		}

		/// <summary>
		/// Add array to object
		/// </summary>
		std::optional<std::shared_ptr<json_value>> obj_add_arr(const std::string &key)
		{
			if (value_kind!=json_value_kind::OBJECT) return {};
			if (obj_has(key)) return {};

			auto val = create<json_value_kind::ARRAY>();
			std::get<object_t>(value).emplace_back(key, val);
			return val;
		}

		/// <summary>
		/// Add string to object
		/// </summary>
		std::optional<std::shared_ptr<json_value>> obj_add_str(const std::string &key, const std::string &string)
		{
			if (value_kind!=json_value_kind::OBJECT) return {};
			if (obj_has(key)) return {};

			auto val = create(string);
			std::get<object_t>(value).emplace_back(key, val);
			return val;
		}

		/// <summary>
		/// Add number to object
		/// </summary>
		std::optional<std::shared_ptr<json_value>> obj_add_num(const std::string &key, number_t number)
		{
			if (value_kind!=json_value_kind::OBJECT) return {};
			if (obj_has(key)) return {};

			auto val = create(number);
			std::get<object_t>(value).emplace_back(key, val);
			return val;
		}

		/// <summary>
		/// Add boolean to object
		/// </summary>
		std::optional<std::shared_ptr<json_value>> obj_add_bool(const std::string &key, bool boolean)
		{
			if (value_kind!=json_value_kind::OBJECT) return {};
			if (obj_has(key)) return {};

			auto val = create(boolean);
			std::get<object_t>(value).emplace_back(key, val);
			return val;
		}

		/// <summary>
		/// Add object to array
		/// </summary>
		std::optional<std::shared_ptr<json_value>> arr_add_obj()
		{
			if (value_kind!=json_value_kind::ARRAY) return {};

			auto obj = create<json_value_kind::OBJECT>();
			std::get<array_t>(value).push_back(obj);
			return obj;
		}

		/// <summary>
		/// Add existing object to array
		/// </summary>
		std::optional<std::shared_ptr<json_value>> arr_add_obj(const std::shared_ptr<json_value> &obj)
		{
			if (value_kind!=json_value_kind::ARRAY) return {};

			std::get<array_t>(value).push_back(obj);
			return obj;
		}

		/// <summary>
		/// Add array to array
		/// </summary>
		std::optional<std::shared_ptr<json_value>> arr_add_arr()
		{
			if (value_kind!=json_value_kind::ARRAY) return {};

			auto val = create<json_value_kind::ARRAY>();
			std::get<array_t>(value).push_back(val);
			return val;
		}

		/// <summary>
		/// Add string to array
		/// </summary>
		std::optional<std::shared_ptr<json_value>> arr_add_str(const std::string &string)
		{
			if (value_kind!=json_value_kind::ARRAY) return {};

			auto val = create(string);
			std::get<array_t>(value).push_back(val);
			return val;
		}

		/// <summary>
		/// Add number to array
		/// </summary>
		std::optional<std::shared_ptr<json_value>> arr_add_num(number_t number)
		{
			if (value_kind!=json_value_kind::ARRAY) return {};

			auto val = create(number);
			std::get<array_t>(value).push_back(val);
			return val;
		}

		/// <summary>
		/// Add boolean to array
		/// </summary>
		std::optional<std::shared_ptr<json_value>> arr_add_bool(bool boolean)
		{
			if (value_kind!=json_value_kind::ARRAY) return {};

			auto val = create(boolean);
			std::get<array_t>(value).push_back(val);
			return val;
		}

		/// <summary>
		/// Get a member from an object
		/// </summary>
		std::optional<std::shared_ptr<json_value>> obj_get(const std::string &key)
		{
			auto &entires = std::get<object_t>(value);
			auto it = std::find_if(entires.begin(), entires.end(),
				[&key](std::pair<std::string, std::shared_ptr<json_value>> pair) {
					return pair.first == key;
				});

			if (it != entires.end()) {
				auto ix = std::distance(entires.begin(), it);
				return entires[ix].second;
			} else {
				return {};
			}
		}

		bool obj_has(const std::string &key) const
		{
			auto &entires = std::get<object_t>(value);
			return std::find_if(entires.begin(), entires.end(),
				[&key](std::pair<std::string, std::shared_ptr<json_value>> pair) {
					return pair.first == key;
			}) != entires.end();
		}

		std::string stringify() const
		{
			std::stringstream ss;

			switch (value_kind) {
			case json_value_kind::OBJECT:
			{
				auto &entries = std::get<object_t>(value);
				ss << '{';
				size_t ix=0;
				for (auto &[k, v] : entries) {
					if (ix++>0) ss << ',';
					ss << '"' << k << "\":" << v->stringify();
				}
				ss << '}';
				break;
			}
			case json_value_kind::ARRAY:
			{
				auto &array = std::get<array_t>(value);
				ss << '[';
				size_t ix=0;
				for (auto &v : array) {
					if (ix++>0) ss << ',';
					ss << v->stringify();
				}
				ss << ']';
				break;
			}
			case json_value_kind::STRING:
				ss << '"' << json_escape_str(std::get<std::string>(value).c_str()) << '"';
				break;
			case json_value_kind::NUMBER:
				ss << std::get<number_t>(value);
				break;
			case json_value_kind::BOOL:
				ss << (std::get<bool>(value) ? "true" : "false");
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

			ss << json_value_kind_str(value_kind);

			switch (value_kind) {
			case json_value_kind::OBJECT:
			{
				auto &entries = std::get<object_t>(value);
				if (entries.size()) {
					for (auto &[k, v] : entries) {
						ss << ',' << v->kind_str();
					}
					ss << ",END";
				}
				break;
			}
			case json_value_kind::ARRAY:
			{
				auto &array = std::get<array_t>(value);
				if (array.size()) {
					for (auto &v : array) {
						ss << ',' << v->kind_str();
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

#endif /* HILL__UTILS__JSON_HH_INCLUDED */
