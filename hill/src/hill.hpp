#ifndef HILL_HPP_INCLUDED
#define HILL_HPP_INCLUDED

#include "parser.hpp"

#include <concepts>
#include <iostream>
#include <vector>

namespace hill {

	namespace concepts {
		template<typename PT, typename LFT>
		concept parser = requires(PT p, std::istream &istr, LFT lexer) {
			{p.parse(istr, lexer)};
			{p.get_rpn()};
		};

		template<typename AT>
		concept analyzer = requires(AT a, std::vector<token> rpn) {
			{a.analyze(rpn)};
			{a.get_main_block()};
		};

		template<typename ET>
		concept evaluator = requires(ET e, block b) {
			{e.evaluate(b)};
		};
	}

	template<typename LFT, typename PT, typename AT, typename ET>
	requires concepts::parser<PT, LFT>
		&& concepts::analyzer<AT>
		&& concepts::evaluator<ET>
	void hill(std::istream &istr, LFT lexer, PT &parser, AT &analyzer, ET &evaluator)
	{
		parser.parse(istr, lexer);
		analyzer.analyze(parser.get_rpn());
		evaluator.evaluate(analyzer.get_main_block());
	}
}

#endif /* HILL_HPP_INCLUDED */
