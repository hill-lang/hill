#ifndef UTILS__CONSOLE_HPP_INCLUDED
#define UTILS__CONSOLE_HPP_INCLUDED

#include <ios>

namespace hill::utils {

	enum class console_color {
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

	inline int current_color = 0;

	inline std::ostream &apply_color(std::ostream &os) {
		return os << "\033[" << current_color << "m";
	}

	/// <summary>
	/// Set the conosle forground color
	/// </summary>
	inline std::ostream &(*setcolor(console_color color))(std::ostream &) {
		current_color = static_cast<int>(color);
		return apply_color;
	}

	/// <summary>
	/// Set the conosle forground color
	/// </summary>
	inline std::ostream &(*setbgcolor(console_color color))(std::ostream &) {
		current_color = static_cast<int>(color) + 10;
		return apply_color;
	}

	/// <summary>
	/// Resets the conosle forground and background color
	/// </summary>
	inline std::ostream &(*resetcolor())(std::ostream &) {
		current_color = static_cast<int>(console_color::NONE);
		return apply_color;
	}
}

#endif /* UTILS__CONSOLE_HPP_INCLUDED */
