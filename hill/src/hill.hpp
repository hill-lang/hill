#ifndef HILL_HPP_INCLUDED
#define HILL_HPP_INCLUDED

#include "parser.hpp"

#include <concepts>
#include <iostream>

namespace hill {

	namespace concepts {
		template<typename PT, typename LFT>
		concept parser = requires(PT p, std::istream & istr, LFT lexer) {
			{ p.parse(istr, lexer) };
			{ p.get_rpn() };
		};

		template<typename AT, typename RPNT>
		concept analyzer = requires(AT a, RPNT rpn) {
			{ a.analyze(rpn) };
			{ a.get_main_block() };
		};

		template<typename ET, typename BT>
		concept evaluator = requires(ET e, BT b) {
			{ e.evaluate(b) };
		};
	}

	template<typename LFT, typename PT, typename AT, typename ET>
	requires concepts::parser<PT, LFT> &&
		concepts::analyzer<AT, decltype(std::declval<PT>().get_rpn())> &&
		concepts::evaluator<ET, decltype(std::declval<AT>().get_main_block())>
	void hill(std::istream &istr, LFT lexer, PT &parser, AT &analyzer, ET &evaluator)
	{
		parser.parse(istr, lexer);
		analyzer.analyze(parser.get_rpn());
		evaluator.evaluate(analyzer.get_main_block());
	}
}

#endif /* HILL_HPP_INCLUDED */
