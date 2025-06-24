#ifndef HILL__UTILS__JSON_PARSER_HH_INCLUDED
#define HILL__UTILS__JSON_PARSER_HH_INCLUDED

// www.json.org

#include "./json.hh"

#include <algorithm>
#include <istream>
#include <memory>
#include <sstream>
#include <optional>

namespace hill::utils {

	constexpr bool is_json_ws(char ch)
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
		json_parser() = delete;

		/// <summary>
		/// Parse a json string
		/// </summary>
		static std::optional<std::shared_ptr<json_value>> parse(std::istream &istr)
		{
			auto value = parse_value(istr);
			if (!value.has_value()) return std::nullopt;

			skip_ws(istr);
			if (!istr.eof()) return std::nullopt;

			return value;
		}

	private:
		static void skip_ws(std::istream &istr)
		{
			while (true) {
				if (istr.eof()) return;

				auto ch = (char)istr.peek();
				if (is_json_ws(ch)) {
					(void)istr.get();
				} else {
					return;
				}
			}
		}

		static std::optional<std::shared_ptr<json_value>> parse_value(std::istream &istr)
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

		static std::optional<std::shared_ptr<json_value>> parse_object(std::istream &istr)
		{
			auto ret = json_value::create<json_value_kind::OBJECT>();

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
				if (ret->obj_has(key.value()->str().value())) return std::nullopt; // Duplicate key

				auto value = parse_value(istr);
				if (!value.has_value()) return std::nullopt;

				std::get<json_value::object_t>(ret->value).emplace_back(key.value()->str().value(), value.value());

				skip_ws(istr);
				if (istr.eof()) return std::nullopt;
				if (istr.peek()==',') (void)istr.get();
			}

			return ret;
		}

		static std::optional<std::shared_ptr<json_value>> parse_array(std::istream &istr)
		{
			auto arr = json_value::create<json_value_kind::ARRAY>();

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
				
				std::get<json_value::array_t>(arr->value).push_back(value.value());

				skip_ws(istr);
				if (istr.eof()) return std::nullopt;
				if (istr.peek()==',') (void)istr.get();
			}

			return arr;
		}

		static std::optional<std::shared_ptr<json_value>> parse_string(std::istream &istr)
		{
			std::stringstream ss;

			while (true) {
				if (istr.eof()) return std::nullopt;

				auto ch = (char)istr.get();
				if (ch == '"') break;

				if (ch == '\\') { // Handle escaped character
					if (istr.eof()) return std::nullopt;

					ch = (char)istr.get();
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
			return json_value::create(str);
		}

		static std::optional<std::shared_ptr<json_value>> parse_number(std::istream &istr)
		{
			std::stringstream ss;

			auto ch = (char)istr.peek();
			while (ch=='-' || ch=='.' || isdigit(ch)) {
				ss.put((char)istr.get());
				ch = (char)istr.peek();
			}

			auto str = ss.str();
			if (!str.size()) {
				return std::nullopt; // Empty number
			}

			char *end = nullptr;
			auto number = std::strtod(str.c_str(), &end);

			return json_value::create(number);
		}

		static std::optional<std::shared_ptr<json_value>> parse_true(std::istream &istr)
		{
			if (!istr.eof() && istr.get()=='r'
					&& !istr.eof() && istr.get()=='u'
					&& !istr.eof() && istr.get()=='e') {
				return json_value::create(true);
			} else {
				return std::nullopt;
			}
		}

		static std::optional<std::shared_ptr<json_value>> parse_false(std::istream &istr)
		{
			if (!istr.eof() && istr.get()=='a'
					&& !istr.eof() && istr.get()=='l'
					&& !istr.eof() && istr.get()=='s'
					&& !istr.eof() && istr.get()=='e') {
				return json_value::create(false);
			} else {
				return std::nullopt;
			}
		}

		static std::optional<std::shared_ptr<json_value>> parse_null(std::istream &istr)
		{
			if (!istr.eof() && istr.get()=='u'
					&& !istr.eof() && istr.get()=='l'
					&& !istr.eof() && istr.get()=='l') {
				return json_value::create<json_value_kind::JSON_NULL>();
			} else {
				return std::nullopt;
			}
		}
	};
}

#endif /* HILL__UTILS__JSON_PARSER_HH_INCLUDED */
