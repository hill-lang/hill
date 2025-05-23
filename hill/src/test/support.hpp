#ifndef TEST__SUPPORT_HPP_INCLUDED
#define TEST__SUPPORT_HPP_INCLUDED

#include <sstream>
#include <fstream>
#include <istream>

namespace hill {

	namespace test {
		std::stringstream get_src(const char *src_spec)
		{
			bool isFileTest = src_spec[0] == ':';

			std::stringstream ss;
			if (isFileTest) {
				std::ifstream fstr(src_spec+1, std::ios_base::binary);
				ss << fstr.rdbuf();
			} else {
				ss << src_spec;
			}

			return ss;
		}
	}
}

#endif /* TEST__SUPPORT_HPP_INCLUDED */
