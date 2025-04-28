#ifndef LANG_SPEC_HPP_INCLUDED
#define LANG_SPEC_HPP_INCLUDED

#include "exceptions.hpp"
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
		OP_BANG,
		OP_STAR,
		OP_STAR_STAR,
		OP_STAR_EQ,
		OP_SLASH,
		OP_SLASH_EQ,
		OP_PERCENT,
		OP_PERCENT_EQ,
		OP_LESS,
		OP_LESS_EQ,
		OP_LESS_EQ_GREATER,
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

	enum class tt_arity {
		NULLARY,
		LUNARY,
		RUNARY,
		BINARY
	};

	enum class tt_kind {
		NO,
		WS,
		VAL,
		OP,
		LGROUP,
		RGROUP
	};

	struct ot_spec {
		int prec;
		tt_assoc assoc;
		tt_arity arity;
	};

	struct tt_spec {
		tt_spec(
				tt_kind kind,
				std::string name,
				std::string op_pattern,
				std::vector<ot_spec> op_specs):
			kind(kind),
			name(name),
			pattern(op_pattern),
			op_specs(op_specs) {}
		const tt_kind kind;
		const std::string name;
		const std::string pattern;
		const std::vector<ot_spec> op_specs;
	};

	struct lang_spec {
		static const lang_spec &get()
		{
			static lang_spec s_instance;
			return s_instance;
		}

		const std::map<std::string, tt> &get_tt_patterns() const {return tt_patterns;}
		const tt_spec &get_tt_spec(tt type) const {return this->tt_specs.at(type);}

	private:
		const std::map<tt, tt_spec> tt_specs = {
			{tt::START, {tt_kind::NO, "START", "", {}}},
			{tt::END, {tt_kind::NO, "END", "", {}}},
			{tt::ERROR, {tt_kind::NO, "ERROR", "", {}}},
			{tt::WHITESPACE, {tt_kind::WS, "WHITESPACE", "", {}}},
			{tt::COMMENT, {tt_kind::WS, "COMMENT", "", {}}},
			{tt::NAME, {tt_kind::VAL, "NAME", "", {}}},
			{tt::NUM, {tt_kind::VAL, "NUM", "", {}}},
			{tt::STRING, {tt_kind::VAL, "STRING", "", {}}},
			{tt::CHAR, {tt_kind::VAL, "CHAR", "", {}}},
			{tt::LPAR, {tt_kind::LGROUP, "LPAR", "(", {}}},
			{tt::RPAR, {tt_kind::RGROUP, "RPAR", ")", {}}},
			{tt::LCURLY, {tt_kind::LGROUP, "LCURLY", "{", {}}},
			{tt::RCURLY, {tt_kind::RGROUP, "RCURLY", "}", {}}},
			{tt::LSQUARE, {tt_kind::LGROUP, "LSQUARE", "[", {}}},
			{tt::RSQUARE, {tt_kind::RGROUP, "RSQUARE", "]", {}}},
			{tt::CALL, {tt_kind::OP, "CALL", "", {}}},
			{tt::OP_PLUS_PLUS, {tt_kind::OP, "OP_PLUS_PLUS", "++", {
				{2, tt_assoc::RIGHT, tt_arity::RUNARY}, /* Postfix increment */
				{3, tt_assoc::LEFT, tt_arity::LUNARY}}}}, /* Prefix increment */
			{tt::OP_MINUS_MINUS, {tt_kind::OP, "OP_MINUS_MINUS", "--", {
				{2, tt_assoc::RIGHT, tt_arity::RUNARY}, /* Postfix increment */
				{3, tt_assoc::LEFT, tt_arity::LUNARY}}}}, /* Prefix increment */
			{tt::OP_DOT, {tt_kind::OP, "OP_DOT", ".", {
				{2, tt_assoc::LEFT, tt_arity::BINARY}}}},
			{tt::OP_PLUS, {tt_kind::OP, "OP_PLUS", "+", {
				{3, tt_assoc::RIGHT, tt_arity::LUNARY}, /* Unary plus */
				{6, tt_assoc::LEFT, tt_arity::BINARY}}}}, /* Addition */
			{tt::OP_MINUS, {tt_kind::OP, "OP_MINUS", "-", {
				{3, tt_assoc::RIGHT, tt_arity::LUNARY}, /* Unary minus */
				{6, tt_assoc::LEFT, tt_arity::BINARY}}}}, /* Subtraction */
			{tt::OP_BANG, {tt_kind::OP, "OP_BANG", "!", {
				{3, tt_assoc::RIGHT, tt_arity::LUNARY}}}},
			{tt::OP_STAR_STAR, {tt_kind::OP, "OP_STAR_STAR", "**", {
				{4, tt_assoc::LEFT, tt_arity::BINARY}}}}, /* Power of */
			{tt::OP_STAR, {tt_kind::OP, "OP_STAR", "*", {
				{5, tt_assoc::LEFT, tt_arity::BINARY}}}}, /* Multiplication */
			{tt::OP_SLASH, {tt_kind::OP, "OP_SLASH", "/", {
				{5, tt_assoc::LEFT, tt_arity::BINARY}}}}, /* Division */
			{tt::OP_PERCENT, {tt_kind::OP, "OP_PERCENT", "%", {
				{5, tt_assoc::LEFT, tt_arity::BINARY}}}},
			{tt::OP_LESS_EQ_GREATER, {tt_kind::OP, "OP_LESS_EQ_GREATER", "<=>", {
				{8, tt_assoc::LEFT, tt_arity::BINARY}}}},
			{tt::OP_LESS, {tt_kind::OP, "OP_LESS", "<", {
				{9, tt_assoc::LEFT, tt_arity::BINARY}}}},
			{tt::OP_LESS_EQ, {tt_kind::OP, "OP_LESS_EQ", "<=", {
				{9, tt_assoc::LEFT, tt_arity::BINARY}}}},
			{tt::OP_GREATER, {tt_kind::OP, "OP_GREATER", ">", {
				{9, tt_assoc::LEFT, tt_arity::BINARY}}}},
			{tt::OP_GREATER_EQ, {tt_kind::OP, "OP_GREATER_EQ", ">=", {
				{9, tt_assoc::LEFT, tt_arity::BINARY}}}},
			{tt::OP_EQ_EQ, {tt_kind::OP, "OP_EQ_EQ", "==", {
				{10, tt_assoc::LEFT, tt_arity::BINARY}}}},
			{tt::OP_BANG_EQ, {tt_kind::OP, "OP_BANG_EQ", "!=", {
				{10, tt_assoc::LEFT, tt_arity::BINARY}}}},
			{tt::OP_EQ, {tt_kind::OP, "OP_EQ", "=", {
				{17, tt_assoc::NONE, tt_arity::BINARY}}}},
			{tt::OP_COLON_EQ, {tt_kind::OP, "OP_COLON_EQ", ":=", {
				{17, tt_assoc::RIGHT, tt_arity::BINARY}}}},
			{tt::OP_PLUS_EQ, {tt_kind::OP, "OP_PLUS_EQ", "+=", {
				{17, tt_assoc::RIGHT, tt_arity::BINARY}}}},
			{tt::OP_MINUS_EQ, {tt_kind::OP, "OP_MINUS_EQ", "-=", {
				{17, tt_assoc::RIGHT, tt_arity::BINARY}}}},
			{tt::OP_STAR_EQ, {tt_kind::OP, "OP_STAR_EQ", "*=", {
				{17, tt_assoc::RIGHT, tt_arity::BINARY}}}},
			{tt::OP_SLASH_EQ, {tt_kind::OP, "OP_SLASH_EQ", "/=", {
				{17, tt_assoc::RIGHT, tt_arity::BINARY}}}},
			{tt::OP_PERCENT_EQ, {tt_kind::OP, "OP_PERCENT_EQ", "%=", {
				{17, tt_assoc::RIGHT, tt_arity::BINARY}}}},
			{tt::OP_OR_GREATER, {tt_kind::OP, "OP_OR_GREATER", "|>", {
				{17, tt_assoc::LEFT, tt_arity::BINARY}}}},
			{tt::OP_COMMA, {tt_kind::OP, "OP_COMMA", ",", {
				{18, tt_assoc::LEFT, tt_arity::BINARY}}}},
			{tt::OP_SEMICOLON, {tt_kind::OP, "OP_SEMICOLON", ";", {
				{19, tt_assoc::LEFT, tt_arity::RUNARY}}}},
		};

		std::map<std::string, tt> gen_patterns()
		{
			std::map<std::string, tt> patterns;
			for (auto &tt_spec : this->tt_specs) {
				patterns[tt_spec.second.pattern] = tt_spec.first;
			}
			return patterns;
		}
		const std::map<std::string, tt> tt_patterns = gen_patterns();
	};
}

#endif /* LANG_SPEC_HPP_INCLUDED */
