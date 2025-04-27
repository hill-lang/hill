#ifndef EXCEPTIONS_HPP_INCLUDED
#define EXCEPTIONS_HPP_INCLUDED

#include <exception>

namespace hill {

	class not_implemented_exception: std::exception {
		const char *what() const noexcept override {return "Not implemented";}
	};

	class semantic_error_exception: std::exception {
		const char *what() const noexcept override {return "Semantic error";}
	};
}

#endif /* EXCEPTIONS_HPP_INCLUDED */
