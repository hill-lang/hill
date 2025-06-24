#ifndef HILL__ANALYZER_HH_INCLUDED
#define HILL__ANALYZER_HH_INCLUDED

#include "lexer.hh"
#include "type.hh"
#include "type_conversion.hh"
#include "instr.hh"
#include "block.hh"

#include <memory>
#include <cstdint>

namespace hill {

	struct analyzer {
		analyzer() = default;

		block main;

		void set_trunk(const std::shared_ptr<scope> &s)
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
			/*for (auto &t: rpn) {
				std::cout << " - " << t.to_str();
			}
			std::cout << std::endl;*/
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

#endif /* HILL__ANALYZER_HH_INCLUDED */
