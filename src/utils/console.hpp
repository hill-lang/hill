#ifndef HILL__UTILS__CONSOLE_HPP_INCLUDED
#define HILL__UTILS__CONSOLE_HPP_INCLUDED

#include "string.hpp"
#include <ios>

namespace hill::utils {

	enum class ccolor {
		NONE = 0,
		BLACK = 30, BRIGHT_BLACK = 90,
		RED = 31, BRIGHT_RED = 91,
		GREEN = 32, BRIGHT_GREEN = 92,
		YELLOW = 33, BRIGHT_YELLOW = 93,
		BLUE = 34, BRIGHT_BLUE = 94,
		MAGENTA = 35, BRIGHT_MAGENTA = 95,
		CYAN = 36, BRIGHT_CYAN = 96,
		WHITE = 37, BRIGHT_WHITE = 97,
	};

	/// <summary>
	/// Set the conosle forground color
	/// </summary>
	inline std::string setcolor(ccolor c)
	{
		std::stringstream ss;
		ss << "\033[" << static_cast<int>(c) << "m";
		return ss.str();
	}

	/// <summary>
	/// Set the conosle forground color
	/// </summary>
	inline std::string setbgcolor(ccolor c)
	{
		std::stringstream ss;
		ss << "\033[" << static_cast<int>(c)+10 << "m";
		return ss.str();
	}

	/// <summary>
	/// Resets the conosle forground and background color
	/// </summary>
	inline std::string resetcolor()
	{
		std::stringstream ss;
		ss << "\033[" << static_cast<int>(ccolor::NONE) << "m";
		return ss.str();
	}

	inline std::string color(const std::string &str, ccolor fg, ccolor bg=ccolor::NONE)
	{
		std::stringstream ss;
		ss << setcolor(fg) << setbgcolor(bg) << str << resetcolor();
		return ss.str();
	}
}

#endif /* HILL__UTILS__CONSOLE_HPP_INCLUDED */
