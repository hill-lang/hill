#ifndef HILL__UTILS__JSON_PARSER_HPP_INCLUDED
#define HILL__UTILS__JSON_PARSER_HPP_INCLUDED

// www.json.org

#include <algorithm>
#include <istream>
#include <string>
#include <sstream>
#include <vector>
#include <memory>
#include <optional>

namespace hill::utils {

	class json_parser_internal_exception : public std::exception {
		const char *what() const noexcept override {return "JSON parser exception";}
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
		default: throw json_parser_internal_exception();
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

		std::shared_ptr<json_value> object(const std::string &key)
		{
			auto it = std::find_if(object_entries.begin(), object_entries.end(),
				[&key](std::pair<std::string, std::shared_ptr<json_value>> pair) {
					return pair.first == key;
				});

			if (it != object_entries.end()) {
				auto ix = std::distance(object_entries.begin(), it);
				return object_entries[ix].second;
			} else {
				return nullptr;
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
				throw json_parser_internal_exception();
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

	inline constexpr bool is_json_ws(char ch)
	{
		switch (ch) {
		case ' ':
		case '\t':
		case '\r':
		case '\n':
			return  true;
		default:
			return false;
		}
	}

	struct json_parser {
		json_parser() = default;

		/// <summary>
		/// Parse a json string
		/// </summary>
		std::optional<std::shared_ptr<json_value>> parse(std::istream &istr)
		{
			auto value = parse_value(istr);
			if (!value.has_value()) return std::nullopt;

			skip_ws(istr);
			if (!istr.eof()) return std::nullopt;

			return value;
		}

	private:
		void skip_ws(std::istream &istr)
		{
			while (true) {
				if (istr.eof()) return;

				auto ch = istr.peek();
				if (is_json_ws(ch)) {
					(void)istr.get();
				} else {
					return;
				}
			}
		}

		std::optional<std::shared_ptr<json_value>> parse_value(std::istream &istr)
		{
			skip_ws(istr);
			if (istr.eof()) return std::nullopt;

			switch (istr.peek()) {
			case '"': (void)istr.get(); return parse_string(istr);
			case '[': (void)istr.get(); return parse_array(istr);
			case '{': (void)istr.get(); return parse_object(istr);
			case 't': (void)istr.get(); return parse_true(istr);
			case 'f': (void)istr.get(); return parse_false(istr);
			case 'n': (void)istr.get(); return parse_null(istr);
			default: return parse_number(istr);
			}
		}

		std::optional<std::shared_ptr<json_value>> parse_object(std::istream &istr)
		{
			auto ret = std::make_shared<json_value>(json_value_kind::OBJECT);

			while (true) {
				skip_ws(istr);
				if (istr.eof()) return std::nullopt;

				auto ch = istr.get();
				if (ch=='}') break;
				if (ch!='"') return std::nullopt; // There has to be a '"' to start the first key

				auto key = parse_string(istr);
				if (!key.has_value()) return std::nullopt;

				skip_ws(istr);
				if (istr.eof()) return std::nullopt;

				if (istr.get()!=':') return std::nullopt; // Member without value?
				if (ret->object_contains(key.value()->string)) return std::nullopt; // Duplicate key

				auto value = parse_value(istr);
				if (!value.has_value()) return std::nullopt;

				ret->object_entries.emplace_back(key.value()->string, value.value());

				skip_ws(istr);
				if (istr.eof()) return std::nullopt;
				if (istr.peek()==',') (void)istr.get();
			}

			return ret;
		}

		std::optional<std::shared_ptr<json_value>> parse_array(std::istream &istr)
		{
			auto arr = std::make_shared<json_value>(json_value_kind::ARRAY);

			while (true) {
				skip_ws(istr);
				if (istr.eof()) return std::nullopt;

				// End of array
				if (istr.peek()==']') {
					(void)istr.get();
					break;
				}

				auto value = parse_value(istr);
				if (!value.has_value()) return std::nullopt;

				arr->array.push_back(value.value());

				skip_ws(istr);
				if (istr.eof()) return std::nullopt;
				if (istr.peek()==',') (void)istr.get();
			}

			return arr;
		}

		std::optional<std::shared_ptr<json_value>> parse_string(std::istream &istr)
		{
			std::stringstream ss;

			while (true) {
				if (istr.eof()) return std::nullopt;

				auto ch = istr.get();
				if (ch == '"') break;

				if (ch == '\\') { // Handle escaped character
					if (istr.eof()) return std::nullopt;

					ch = istr.get();
					switch (ch) {
					case '"':
					case '\\':
					case '/':
						ss.put(ch);
						break;
					case 'n': ss.put('\n'); break;
					case 't': ss.put('\t'); break;
					case 'r': ss.put('\r'); break;
					case 'b': ss.put('\b'); break;
					case 'f': ss.put('\f'); break;
					case 'u': // Handle \uHHHH (Unicode)
						return std::nullopt; // Unicode escape sequences not supported
					default:
						return std::nullopt; // Invalid escape sequence
					}
				} else {
					ss.put(ch);
				}
			}

			auto str = ss.str();
			return std::make_shared<json_value>(json_value_kind::STRING, str);
		}

		std::optional<std::shared_ptr<json_value>> parse_number(std::istream &istr)
		{
			std::stringstream ss;

			auto ch = istr.peek();
			while (ch=='-' || ch=='.' || isdigit(ch)) {
				ss.put(istr.get());
				ch = istr.peek();
			}

			auto str = ss.str();
			if (!str.size()) {
				return std::nullopt; // Empty number
			}

			char *end = nullptr;
			double number = std::strtod(str.c_str(), &end);

			return std::make_shared<json_value>(json_value_kind::NUMBER, number);
		}

		std::optional<std::shared_ptr<json_value>> parse_true(std::istream &istr)
		{
			if (!istr.eof() && istr.get()=='r'
					&& !istr.eof() && istr.get()=='u'
					&& !istr.eof() && istr.get()=='e') {
				return std::make_shared<json_value>(json_value_kind::BOOL, true);
			} else {
				return std::nullopt;
			}
		}

		std::optional<std::shared_ptr<json_value>> parse_false(std::istream &istr)
		{
			if (!istr.eof() && istr.get()=='a'
					&& !istr.eof() && istr.get()=='l'
					&& !istr.eof() && istr.get()=='s'
					&& !istr.eof() && istr.get()=='e') {
				return std::make_shared<json_value>(json_value_kind::BOOL, false);
			} else {
				return std::nullopt;
			}
		}

		std::optional<std::shared_ptr<json_value>> parse_null(std::istream &istr)
		{
			if (!istr.eof() && istr.get()=='u'
					&& !istr.eof() && istr.get()=='l'
					&& !istr.eof() && istr.get()=='l') {
				return std::make_shared<json_value>(json_value_kind::JSON_NULL);
			} else {
				return std::nullopt;
			}
		}
	};
}

#endif /* HILL__UTILS__JSON_PARSER_HPP_INCLUDED */
