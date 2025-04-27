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
				std::string op_pattern=""):
			prec(precedence),
			assoc(assoc),
			arity(arity),
			kind(kind) {}
		int prec;
		tt_assoc assoc;
		int arity;
		tt_kind kind;
	};

	struct lang_spec {
		static const lang_spec &get()
		{
			static lang_spec s_instance;
			return s_instance;
		}

		const std::map<std::string, tt> &get_tt_patterns() const {return tt_patterns;}
		const std::vector<tt_spec> &get_tt_specs(tt type) const {return this->tt_specs.at(type);}
		
	private:
		const std::map<tt, std::vector<tt_spec>> tt_specs = {
			{tt::START, {tt_spec(0, tt_assoc::NONE, tt_arity::NULLARY, tt_kind::NO)}},
			{tt::END, {tt_spec(0, tt_assoc::NONE, tt_arity::NULLARY, tt_kind::NO)}},
			{tt::ERROR, {tt_spec(0, tt_assoc::NONE, tt_arity::NULLARY, tt_kind::NO)}},
			{tt::WHITESPACE, {tt_spec(0, tt_assoc::NONE, tt_arity::NULLARY, tt_kind::WS)}},
			{tt::COMMENT, {tt_spec(0, tt_assoc::NONE, tt_arity::NULLARY, tt_kind::WS)}},
			{tt::NAME, {tt_spec(0, tt_assoc::NONE, tt_arity::NULLARY, tt_kind::VAL)}},
			{tt::NUM, {tt_spec(0, tt_assoc::NONE, tt_arity::NULLARY, tt_kind::VAL)}},
			{tt::STRING, {tt_spec(0, tt_assoc::NONE, tt_arity::NULLARY, tt_kind::VAL)}},
			{tt::CHAR, {tt_spec(0, tt_assoc::NONE, tt_arity::NULLARY, tt_kind::VAL)}},
			{tt::LPAR, {tt_spec(0, tt_assoc::NONE, tt_arity::NULLARY, tt_kind::LGROUP)}},
			{tt::RPAR, {tt_spec(0, tt_assoc::NONE, tt_arity::NULLARY, tt_kind::RGROUP)}},
			{tt::LCURLY, {tt_spec(0, tt_assoc::NONE, tt_arity::NULLARY, tt_kind::LGROUP)}},
			{tt::RCURLY, {tt_spec(0, tt_assoc::NONE, tt_arity::NULLARY, tt_kind::RGROUP)}},
			{tt::LSQUARE, {tt_spec(0, tt_assoc::NONE, tt_arity::NULLARY, tt_kind::LGROUP)}},
			{tt::RSQUARE, {tt_spec(0, tt_assoc::NONE, tt_arity::NULLARY, tt_kind::RGROUP)}},
			{tt::CALL, {tt_spec(2, tt_assoc::NONE, tt_arity::BINARY, tt_kind::OP)}},
			{tt::OP_PLUS_PLUS, {
				tt_spec(2, tt_assoc::RIGHT, tt_arity::RUNARY, tt_kind::OP), /* Postfix increment */
				tt_spec(3, tt_assoc::LEFT, tt_arity::LUNARY, tt_kind::OP)}}, /* Prefix increment */
			{tt::OP_MINUS_MINUS, {
				tt_spec(2, tt_assoc::RIGHT, tt_arity::RUNARY, tt_kind::OP), /* Postfix increment */
				tt_spec(3, tt_assoc::LEFT, tt_arity::LUNARY, tt_kind::OP)}}, /* Prefix increment */
			{tt::OP_DOT, {tt_spec(2, tt_assoc::LEFT, tt_arity::BINARY, tt_kind::OP)}},
			{tt::OP_PLUS, {
				tt_spec(3, tt_assoc::RIGHT, tt_arity::LUNARY, tt_kind::OP), /* Unary plus */
				tt_spec(6, tt_assoc::LEFT, tt_arity::BINARY, tt_kind::OP)}}, /* Addition */
			{tt::OP_MINUS, {
				tt_spec(3, tt_assoc::RIGHT, tt_arity::LUNARY, tt_kind::OP), /* Unary minus */
				tt_spec(6, tt_assoc::LEFT, tt_arity::BINARY, tt_kind::OP)}}, /* Subtraction */
			{tt::OP_BANG, {tt_spec(3, tt_assoc::RIGHT, tt_arity::LUNARY, tt_kind::OP)}},
			{tt::OP_STAR_STAR, {tt_spec(4, tt_assoc::LEFT, tt_arity::BINARY, tt_kind::OP)}}, /* Power of */
			{tt::OP_STAR, {tt_spec(5, tt_assoc::LEFT, tt_arity::BINARY, tt_kind::OP)}}, /* Multiplication */
			{tt::OP_SLASH, {tt_spec(5, tt_assoc::LEFT, tt_arity::BINARY, tt_kind::OP)}}, /* Division */
			{tt::OP_PERCENT, {tt_spec(5, tt_assoc::LEFT, tt_arity::BINARY, tt_kind::OP)}},
			{tt::OP_LESS, {tt_spec(9, tt_assoc::LEFT, tt_arity::BINARY, tt_kind::OP)}},
			{tt::OP_LESS_EQ, {tt_spec(9, tt_assoc::LEFT, tt_arity::BINARY, tt_kind::OP)}},
			{tt::OP_GREATER, {tt_spec(9, tt_assoc::LEFT, tt_arity::BINARY, tt_kind::OP)}},
			{tt::OP_GREATER_EQ, {tt_spec(9, tt_assoc::LEFT, tt_arity::BINARY, tt_kind::OP)}},
			{tt::OP_EQ_EQ, {tt_spec(10, tt_assoc::LEFT, tt_arity::BINARY, tt_kind::OP)}},
			{tt::OP_BANG_EQ, {tt_spec(10, tt_assoc::LEFT, tt_arity::BINARY, tt_kind::OP)}},
			{tt::OP_EQ, {tt_spec(17, tt_assoc::NONE, tt_arity::BINARY, tt_kind::OP)}},
			{tt::OP_COLON_EQ, {tt_spec(17, tt_assoc::NONE, tt_arity::BINARY, tt_kind::OP)}},
			{tt::OP_PLUS_EQ, {tt_spec(17, tt_assoc::NONE, tt_arity::BINARY, tt_kind::OP)}},
			{tt::OP_MINUS_EQ, {tt_spec(17, tt_assoc::NONE, tt_arity::BINARY, tt_kind::OP)}},
			{tt::OP_STAR_EQ, {tt_spec(17, tt_assoc::NONE, tt_arity::BINARY, tt_kind::OP)}},
			{tt::OP_SLASH_EQ, {tt_spec(17, tt_assoc::NONE, tt_arity::BINARY, tt_kind::OP)}},
			{tt::OP_PERCENT_EQ, {tt_spec(17, tt_assoc::NONE, tt_arity::BINARY, tt_kind::OP)}},
			{tt::OP_OR_GREATER, {tt_spec(17, tt_assoc::NONE, tt_arity::BINARY, tt_kind::OP)}},
			{tt::OP_COMMA, {tt_spec(18, tt_assoc::LEFT, tt_arity::BINARY, tt_kind::OP)}},
			{tt::OP_SEMICOLON, {tt_spec(19, tt_assoc::LEFT, tt_arity::RUNARY, tt_kind::OP)}},
		};

		const std::map<std::string, tt> tt_patterns = {
			{"(", tt::LPAR},
			{")", tt::RPAR},
			{"{", tt::LCURLY},
			{"}", tt::RCURLY},
			{"[", tt::LSQUARE},
			{"]", tt::RSQUARE},
			{"++", tt::OP_PLUS_PLUS},
			{"--", tt::OP_MINUS_MINUS},
			{".", tt::OP_DOT},
			{"+", tt::OP_PLUS},
			{"-", tt::OP_MINUS},
			{"!", tt::OP_BANG},
			{"**", tt::OP_STAR_STAR},
			{"*", tt::OP_STAR},
			{"/", tt::OP_SLASH},
			{"%", tt::OP_PERCENT},
			{"<", tt::OP_LESS},
			{"<=", tt::OP_LESS_EQ},
			{">", tt::OP_GREATER},
			{">=", tt::OP_GREATER_EQ},
			{"==", tt::OP_EQ_EQ},
			{"!=", tt::OP_BANG_EQ},
			{"=", tt::OP_EQ},
			{":=", tt::OP_COLON_EQ},
			{"+=", tt::OP_PLUS_EQ},
			{"-=", tt::OP_MINUS_EQ},
			{"*=", tt::OP_STAR_EQ},
			{"/=", tt::OP_SLASH_EQ},
			{"%=", tt::OP_PERCENT_EQ},
			{"|>", tt::OP_OR_GREATER},
			{",", tt::OP_COMMA},
			{";", tt::OP_SEMICOLON},
		};
	};

	inline const char *tt_to_str(tt token_type)
	{
		switch (token_type) {
		case tt::START: return "START";
		case tt::WHITESPACE: return "WHITESPACE";
		case tt::COMMENT: return "COMMENT";
		case tt::NAME: return "NAME";
		case tt::NUM: return "NUM";
		case tt::CHAR: return "CHAR";
		case tt::STRING: return "STRING";
		case tt::LPAR: return "LPAR";
		case tt::RPAR: return "RPAR";
		case tt::LCURLY: return "LCURLY";
		case tt::RCURLY: return "RCURLY";
		case tt::LSQUARE: return "LSQUARE";
		case tt::RSQUARE: return "RSQUARE";
		case tt::CALL: return "CALL";
		case tt::OP_DOT: return "OP_DOT";
		case tt::OP_PLUS: return "OP_PLUS";
		case tt::OP_PLUS_PLUS: return "OP_PLUS_PLUS";
		case tt::OP_PLUS_EQ: return "OP_PLUS_EQ";
		case tt::OP_MINUS: return "OP_MINUS";
		case tt::OP_MINUS_MINUS: return "OP_MINUS_MINUS";
		case tt::OP_MINUS_EQ: return "OP_MINUS_EQ";
		case tt::OP_BANG: return "OP_BANG";
		case tt::OP_STAR: return "OP_STAR";
		case tt::OP_STAR_STAR: return "OP_STAR_STAR";
		case tt::OP_STAR_EQ: return "OP_STAR_EQ";
		case tt::OP_SLASH: return "OP_SLASH";
		case tt::OP_SLASH_EQ: return "OP_SLASH_EQ";
		case tt::OP_PERCENT: return "OP_PERCENT";
		case tt::OP_PERCENT_EQ: return "OP_PERCENT_EQ";
		case tt::OP_LESS: return "OP_LESS";
		case tt::OP_LESS_EQ: return "OP_LESS_EQ";
		case tt::OP_GREATER: return "OP_GREATER";
		case tt::OP_GREATER_EQ: return "OP_GREATER_EQ";
		case tt::OP_EQ_EQ: return "OP_EQ_EQ";
		case tt::OP_BANG_EQ: return "OP_BANG_EQ";
		case tt::OP_EQ: return "OP_EQ";
		case tt::OP_OR_GREATER: return "OP_OR_GREATER";
		case tt::OP_COLON_EQ: return "OP_COLON_EQ";
		case tt::OP_SEMICOLON: return "OP_SEMICOLON";
		case tt::OP_COMMA: return "OP_COMMA";
		case tt::ERROR: return "ERROR";
		case tt::END: return "END";
		default: throw not_implemented_exception();
		}
	}
}

#endif /* LANG_SPEC_HPP_INCLUDED */
