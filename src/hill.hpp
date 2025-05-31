#ifndef HILL__HILL_HPP_INCLUDED
#define HILL__HILL_HPP_INCLUDED

#include "parser.hpp"
#include "serializer.hpp"
#include "value.hpp"

#include <concepts>
#include <iostream>
#include <vector>

namespace hill {

	namespace concepts {
		template<typename PT, typename LT>
		concept parser = requires(PT p, std::istream &istr, LT &lexer) {
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

	scope build_root()
	{
		return scope();
	}

	scope build_lib(scope *parent)
	{
		scope s;

		int32_t a = 42;
		s.ids["the_answer"] = val_ref(a, basic_type::I32);

		s.parent = parent;

		return s;
	}

	template<typename LT, typename PT, typename AT, typename ET>
	requires concepts::parser<PT, LT>
		&& concepts::analyzer<AT>
		&& concepts::evaluator<ET>
	value hill(std::istream &istr, LT &lexer, PT &parser, AT &analyzer, ET &evaluator)
	{
		parser.parse(istr, lexer);

		scope root = build_root();
		scope lib = build_lib(&root);
		analyzer.set_trunk(&lib);
		analyzer.analyze(parser.get_rpn());

		// Debug only!
		serializer s(serializer_mode::ASCII);
		s.serialize("./output.hill_c.txt", analyzer.get_main_block());
		s = serializer(serializer_mode::BIN);
		s.serialize("./output.hill_c", analyzer.get_main_block());

		return evaluator.evaluate(analyzer.get_main_block());
	}
}

#endif /* HILL__HILL_HPP_INCLUDED */
