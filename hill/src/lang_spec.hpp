#ifndef LANG_SPEC_HPP_INCLUDED
#define LANG_SPEC_HPP_INCLUDED

#include <map>
#include <string>
#include <vector>
#include <memory>

namespace hill {

	enum class tt {
		START,
		WHITESPACE,
		COMMENT,
		NAME,
		NUM,
		CHAR,
		STRING,
		//OP,
		LPAR,
		RPAR,
		LCURLY,
		RCURLY,
		LSQUARE,
		RSQUARE,
		CALL,
		OP_DOT,
		OP_PLUS,
		OP_PLUS_PLUS,
		OP_PLUS_EQ,
		OP_MINUS,
		OP_MINUS_MINUS,
		OP_MINUS_EQ,
		OP_STAR,
		OP_STAR_STAR,
		OP_STAR_EQ,
		OP_SLASH,
		OP_SLASH_EQ,
		OP_PERCENT,
		OP_PERCENT_EQ,
		OP_LESS,
		OP_LESS_EQ,
		OP_GREATER,
		OP_GREATER_EQ,
		OP_EQ_EQ,
		OP_BANG_EQ,
		OP_EQ,
		OP_OR_GREATER,
		OP_COLON_EQ,
		OP_SEMICOLON,
		OP_COMMA,
		ERROR,
		END
	};

	enum class tt_assoc {
		LEFT,
		RIGHT,
		NONE
	};

	namespace tt_arity {
		enum tt_arity: int {
			NULLARY=1,
			LUNARY=2,
			RUNARY=4,
			BINARY=8
		};
	}

	enum class tt_kind {
		NO,
		WS,
		VAL,
		OP,
		LGROUP,
		RGROUP
	};

	struct tt_spec {
		tt_spec(
				int precedence,
				tt_assoc assoc,
				int arity,
				tt_kind kind,
				std::string op_pattern="",
				std::string name=""):
			prec(precedence),
			assoc(assoc),
			arity(arity),
			kind(kind),
			op_pattern(op_pattern),
			name(name) {}
		int prec;
		tt_assoc assoc;
		int arity;
		tt_kind kind;
		std::string op_pattern;
		std::string name;
	};

	struct lang_spec {
		lang_spec()
		{
			// Build tt patterns
			for (const auto &ts : tt_specs) {
				if (!ts.second.op_pattern.empty()) {
					this->tt_patterns[ts.second.op_pattern] = ts.first;
				}
			}
		}

		static const lang_spec &get()
		{
			static lang_spec s_instance;
			return s_instance;
		}

		const std::map<std::string, tt> &get_tt_patterns() const {return tt_patterns;}
		const tt_spec *get_tt_spec(tt type) const
		{
#if _DEBUG // Not really needed imo
			if (!this->tt_specs.contains(type)) {
				throw;
			}
#endif
			return &this->tt_specs.at(type);
		}

	private:
		const std::map<tt, tt_spec> tt_specs = {
			{tt::START, tt_spec(0, tt_assoc::NONE, tt_arity::NULLARY, tt_kind::NO, "", "START")},
			{tt::END, tt_spec(0, tt_assoc::NONE, tt_arity::NULLARY, tt_kind::NO, "", "END")},
			{tt::ERROR, tt_spec(0, tt_assoc::NONE, tt_arity::NULLARY, tt_kind::NO, "", "ERROR")},
			{tt::WHITESPACE, tt_spec(0, tt_assoc::NONE, tt_arity::NULLARY, tt_kind::WS, "", "WHITESPACE")},
			{tt::COMMENT, tt_spec(0, tt_assoc::NONE, tt_arity::NULLARY, tt_kind::WS, "", "COMMENT")},
			{tt::NAME, tt_spec(0, tt_assoc::NONE, tt_arity::NULLARY, tt_kind::VAL, "", "NAME")},
			{tt::NUM, tt_spec(0, tt_assoc::NONE, tt_arity::NULLARY, tt_kind::VAL, "", "NUM")},
			{tt::STRING, tt_spec(0, tt_assoc::NONE, tt_arity::NULLARY, tt_kind::VAL, "", "STRING")},
			{tt::CHAR, tt_spec(0, tt_assoc::NONE, tt_arity::NULLARY, tt_kind::VAL, "", "CHAR")},
			{tt::LPAR, tt_spec(0, tt_assoc::NONE, tt_arity::NULLARY, tt_kind::LGROUP, "(", "LPAR")},
			{tt::RPAR, tt_spec(0, tt_assoc::NONE, tt_arity::NULLARY, tt_kind::RGROUP, ")", "RPAR")},
			{tt::LCURLY, tt_spec(0, tt_assoc::NONE, tt_arity::NULLARY, tt_kind::LGROUP, "{", "LCURLY")},
			{tt::RCURLY, tt_spec(0, tt_assoc::NONE, tt_arity::NULLARY, tt_kind::RGROUP, "}", "RCURLY")},
			{tt::LSQUARE, tt_spec(0, tt_assoc::NONE, tt_arity::NULLARY, tt_kind::LGROUP, "[", "LSQUARE")},
			{tt::RSQUARE, tt_spec(0, tt_assoc::NONE, tt_arity::NULLARY, tt_kind::RGROUP, "]", "RSQUARE")},
			{tt::CALL, tt_spec(2, tt_assoc::NONE, tt_arity::BINARY, tt_kind::OP, "", "CALL")},
			{tt::OP_DOT, tt_spec(2, tt_assoc::LEFT, tt_arity::BINARY, tt_kind::OP, ".", "OP_DOT")},
			{tt::OP_PLUS, tt_spec(3, tt_assoc::RIGHT, tt_arity::BINARY, tt_kind::OP, "+", "OP_PLUS")},
			{tt::OP_MINUS, tt_spec(3, tt_assoc::RIGHT, tt_arity::BINARY|tt_arity::LUNARY, tt_kind::OP, "-", "OP_MINUS")},
			{tt::OP_STAR, tt_spec(5, tt_assoc::RIGHT, tt_arity::BINARY, tt_kind::OP, "*", "OP_STAR")},
			{tt::OP_SLASH, tt_spec(5, tt_assoc::RIGHT, tt_arity::BINARY, tt_kind::OP, "/", "OP_SLASH")},
			{tt::OP_PERCENT, tt_spec(5, tt_assoc::RIGHT, tt_arity::BINARY, tt_kind::OP, "%", "OP_PERCENT")},
			{tt::OP_EQ, tt_spec(17, tt_assoc::NONE, tt_arity::BINARY, tt_kind::OP, "=", "OP_EQ")},
			{tt::OP_COLON_EQ, tt_spec(17, tt_assoc::NONE, tt_arity::BINARY, tt_kind::OP, ":=", "OP_COLON_EQ")},
			{tt::OP_OR_GREATER, tt_spec(17, tt_assoc::NONE, tt_arity::BINARY, tt_kind::OP, "|>", "OP_OR_GREATER")},
			{tt::OP_COMMA, tt_spec(18, tt_assoc::NONE, tt_arity::BINARY, tt_kind::OP, ",", "OP_COMMA")},
			{tt::OP_SEMICOLON, tt_spec(19, tt_assoc::LEFT, tt_arity::RUNARY, tt_kind::OP, ";", "OP_SEMICOLON")},
		};

		std::map<std::string, tt> tt_patterns;
	};
}

#endif /* LANG_SPEC_HPP_INCLUDED */
