#ifndef HILL__UTILS__JSON_PARSER_HPP_INCLUDED
#define HILL__UTILS__JSON_PARSER_HPP_INCLUDED

// www.json.org

#include <algorithm>
#include <istream>
#include <string>
#include <sstream>
#include <vector>
#include <memory>

namespace hill::utils {

	class json_parser_exception : public std::exception {
		const char *what() const noexcept override { return "JSON parser exception"; }
	};

	class json_parser_internal_exception : public std::exception {
		const char *what() const noexcept override { return "JSON parser internal exception"; }
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

		std::vector<std::pair<std::string, std::shared_ptr<json_value>>> object_entires;
		std::vector<std::shared_ptr<json_value>> array;
		std::string string;
		double number = 0.0;
		bool boolean = false;

		std::shared_ptr<json_value> object(const std::string &key)
		{
			auto it = std::find_if(object_entires.begin(), object_entires.end(),
				[&key](std::pair<std::string, std::shared_ptr<json_value>> pair) {
					return pair.first == key;
				});

			if (it != object_entires.end()) {
				auto ix = std::distance(object_entires.begin(), it);
				return object_entires[ix].second;
			} else {
				return nullptr;
			}
		}

		bool object_contains(const std::string &key) const
		{
			return std::find_if(object_entires.begin(), object_entires.end(),
				[&key](std::pair<std::string, std::shared_ptr<json_value>> pair) {
					return pair.first == key;
			}) != object_entires.end();
		}

		std::string to_str()
		{
			std::stringstream ss;

			switch (kind) {
			case json_value_kind::OBJECT:
			{
				ss << '{';
				size_t ix=0;
				for (auto &[key, value] : object_entires) {
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

		std::string kind_str()
		{
			std::stringstream ss;

			ss << json_value_kind_str(kind);

			switch (kind) {
			case json_value_kind::OBJECT:
			{
				if (object_entires.size()) {
					for (auto &[key, value] : object_entires) {
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
		/// <returns>A JSON value if successfull, otherwise nullptr.</returns>
		std::shared_ptr<json_value> parse(std::istream &istr)
		{
			try {
				return parse_value(istr);
			} catch (const json_parser_exception) {
				return nullptr;
			}
		}

	private:
		void skip_ws(std::istream &istr)
		{
			while (true) {
				auto ch = istr.peek();
				if (is_json_ws(ch)) {
					(void)istr.get();
				} else {
					return;
				}
			}
		}

		std::shared_ptr<json_value> parse_object(std::istream &istr)
		{
			auto ret = std::make_shared<json_value>(json_value_kind::OBJECT);

			skip_ws(istr);

			int ch;
			while ((ch=istr.get()) != '}') {
				if (ch!='"') throw json_parser_exception();

				auto member_name = parse_string(istr);
				skip_ws(istr);

				if (istr.get()!=':') throw json_parser_exception();
				if (ret->object_contains(member_name->string)) throw json_parser_exception();

				ret->object_entires.emplace_back(member_name->string, parse_value(istr));

				skip_ws(istr);
				if (istr.peek()==',') (void)istr.get();
				skip_ws(istr);
			}

			return ret;
		}

		std::shared_ptr<json_value> parse_array(std::istream &istr)
		{
			auto arr = std::make_shared<json_value>(json_value_kind::ARRAY);

			skip_ws(istr);

			while (istr.peek()!=']') {
				arr->array.push_back(parse_value(istr));
				skip_ws(istr);
				if (istr.peek()==',') (void)istr.get();
				skip_ws(istr);
			}
			(void)istr.get();

			return arr;
		}

		std::shared_ptr<json_value> parse_value(std::istream &istr)
		{
			skip_ws(istr);

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

		std::shared_ptr<json_value> parse_string(std::istream &istr)
		{
			std::stringstream ss;

			int ch;
			while ((ch=istr.get()) != '"') {
				// Skip escaped character
				if (ch != '\\') {
					ss.put(ch);
				}
			}

			auto str = ss.str();
			return std::make_shared<json_value>(json_value_kind::STRING, str);
		}

		std::shared_ptr<json_value> parse_number(std::istream &istr)
		{
			std::stringstream ss;

			auto ch = istr.peek();
			while (ch=='-' || ch=='.' || isdigit(ch)) {
				ss.put(istr.get());
				ch = istr.peek();
			}

			auto str = ss.str();
			if (!str.size()) {
				throw json_parser_exception();
			}

			char *end = nullptr;
			double number = std::strtod(str.c_str(), &end);

			return std::make_shared<json_value>(json_value_kind::NUMBER, number);
		}

		std::shared_ptr<json_value> parse_true(std::istream &istr)
		{
			if (istr.get()=='r' && istr.get()=='u' && istr.get()=='e') {
				return std::make_shared<json_value>(json_value_kind::BOOL, true);
			} else {
				throw json_parser_exception();
			}
		}

		std::shared_ptr<json_value> parse_false(std::istream &istr)
		{
			if (istr.get()=='a' && istr.get()=='l' && istr.get()=='s' && istr.get()=='e') {
				return std::make_shared<json_value>(json_value_kind::BOOL, false);
			} else {
				throw json_parser_exception();
			}
		}

		std::shared_ptr<json_value> parse_null(std::istream &istr)
		{
			if (istr.get()=='u' && istr.get()=='l' && istr.get()=='l') {
				return std::make_shared<json_value>(json_value_kind::JSON_NULL);
			} else {
				throw json_parser_exception();
			}
		}
	};
}

#endif /* HILL__UTILS__JSON_PARSER_HPP_INCLUDED */
