#ifndef HILL__ANALYZER_HPP_INCLUDED
#define HILL__ANALYZER_HPP_INCLUDED

#include "lexer.hpp"
#include "type.hpp"
#include "type_conversion.hpp"
#include "instr.hpp"
#include "block.hpp"

#include <memory>
#include <cstdint>

namespace hill {

	struct analyzer {
		analyzer() = default;

		block main;

		void set_trunk(scope *s)
		{
			main.s.parent = s;
		}

		void analyze_token(const token &t)
		{
			//std::cout<<"analyzer: "<< t.to_str() <<'\n';

			main.add(t);
		}

		void analyze(const std::vector<token> &rpn)
		{
			for (auto &t: rpn) {
				analyze_token(t);
			}

			//std::cout << main.to_str();
		}

		const block &get_main_block() const
		{
			return main;
		}
	};
}

#endif /* HILL__ANALYZER_HPP_INCLUDED */
