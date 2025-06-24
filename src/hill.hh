#ifndef HILL__HILL_HH_INCLUDED
#define HILL__HILL_HH_INCLUDED

#include "parser.hh"
#include "serializer.hh"
#include "token.hh"
#include "value.hh"

#include <cmath>
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

	inline void pf_abs(uint8_t *rp, const uint8_t *ap)
	{
		*((int32_t *)rp) = (int32_t)std::abs(*((int32_t *)ap));
	}

	inline void pf_pow(uint8_t *rp, const uint8_t *ap)
	{
		*((int32_t *)rp) = (int32_t)std::pow(*((int32_t *)ap), *(((int32_t *)ap)+1));
	}

	inline void pf_dpow(uint8_t *rp, const uint8_t *ap)
	{
		*((double *)rp) = (double)std::pow(*((double *)ap), *(((double *)ap)+1));
	}

	inline void pf_div(uint8_t *rp, uint8_t *ap)
	{
		*((int32_t *)rp) = (int32_t)*((int32_t *)ap) / *(((int32_t *)ap)+1);
	}

	inline void pf_select(uint8_t *rp, uint8_t *ap)
	{
		(void)ap;
		(void)rp;
		// TODO: Type inference
		// TODO: Block support
	}

	std::shared_ptr<scope> build_lib(const std::shared_ptr<scope> &parent)
	{
		auto s = scope::create(parent);

		s->ids["the_answer"].push_back(val_ref((int32_t)42, basic_type::I32));
		s->ids["abs"].push_back(val_ref((void *)pf_abs, type({
			basic_type::FUNC,
			basic_type::I32,
			basic_type::I32,
			basic_type::END})));
		s->ids["pow"].push_back(val_ref((void *)pf_pow, type({
			basic_type::FUNC,
			basic_type::I32,
			basic_type::TUPLE,
			basic_type::I32,
			basic_type::I32,
			basic_type::END})));
		s->ids["pow"].push_back(val_ref((void *)pf_dpow, type({
			basic_type::FUNC,
			basic_type::F64,
			basic_type::TUPLE,
			basic_type::F64,
			basic_type::F64,
			basic_type::END})));
		s->ids["div"].push_back(val_ref((void *)pf_div, type({
			basic_type::FUNC,
			basic_type::I32,
			basic_type::TUPLE,
			basic_type::I32,
			basic_type::I32,
			basic_type::END})));
		s->ids["select"].push_back(val_ref((void *)pf_select, type({
			basic_type::FUNC,
			basic_type::ARRAY,
			basic_type::I32,
			(basic_type)-1, // Placeholder for actual value
			basic_type::END,
			basic_type::TUPLE,
			basic_type::ARRAY,
			basic_type::I32,
			(basic_type)-1,
			basic_type::END,
			basic_type::BLOCK,
			// TODO: Block type inference
			basic_type::END,
			basic_type::END,
			basic_type::END})));
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

#endif /* HILL__HILL_HH_INCLUDED */
