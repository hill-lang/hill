#ifndef HILL__HILL_HPP_INCLUDED
#define HILL__HILL_HPP_INCLUDED

#include "parser.hpp"
#include "serializer.hpp"
#include "token.hpp"
#include "value.hpp"

#include <concepts>
#include <iostream>
#include <vector>
#include <memory>

namespace hill {

	namespace concepts {
		template<typename PT, typename LT>
		concept parser = requires(PT p, std::istream &istr, LT &lexer) {
			{p.parse(istr, lexer)};
			{p.get_rpn()};
		};

		template<typename AT>
		concept analyzer = requires(AT a, std::vector<token> rpn, const std::shared_ptr<scope> &root) {
			{a.analyze(rpn)};
			{a.set_trunk(root)};
			{a.get_main_block()};
		};

		template<typename ET>
		concept evaluator = requires(ET e, block b) {
			{e.evaluate(b)};
		};
	}

	std::shared_ptr<scope> build_root()
	{
		auto s = scope::create();

		// TODO: Add basic types

		return s;
	}

	int32_t pf_abs(int32_t v)
	{
		return (int32_t)abs(v);
	}

	std::shared_ptr<scope> build_lib(const std::shared_ptr<scope> &parent)
	{
		auto s = scope::create(parent);

		s->ids["the_answer"] = val_ref((int32_t)42, basic_type::I32);
		s->ids["abs"] = val_ref((void *)pf_abs, type_spec({basic_type::FUNC, basic_type::I32, basic_type::I32, basic_type::END}));

		return s;
	}

	template<typename LT, typename PT, typename AT, typename ET>
	requires concepts::parser<PT, LT>
		&& concepts::analyzer<AT>
		&& concepts::evaluator<ET>
	value hill(std::istream &istr, LT &lexer, PT &parser, AT &analyzer, ET &evaluator)
	{
		parser.parse(istr, lexer);

		auto root = build_root();
		auto lib = build_lib(root);
		analyzer.set_trunk(lib);
		analyzer.analyze(parser.get_rpn());

		// Debug only!
		std::filesystem::create_directories("./tmp/");
		serializer s(serializer_mode::ASCII);
		s.serialize("./tmp/output.hill_c.txt", analyzer.get_main_block());
		s = serializer(serializer_mode::BIN);
		s.serialize("./tmp/output.hill_c", analyzer.get_main_block());

		return evaluator.evaluate(analyzer.get_main_block());
	}
}

#endif /* HILL__HILL_HPP_INCLUDED */
