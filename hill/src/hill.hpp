#ifndef HILL_HPP_INCLUDED
#define HILL_HPP_INCLUDED

#include "parser.hpp"

#include <iostream>

namespace hill {

	template<typename LFT, typename PT, typename AT, typename ET>
	void hill(std::istream &istr, LFT lexer, PT &parser, AT &analyzer, ET &evaluator)
	{
		parser.parse(istr, lexer);

		analyzer.analyze(parser.get_rpn());

		evaluator.evaluate(analyzer.get_instrs());
	}
}

#endif

