#ifndef HILL__EXCEPTIONS_HPP_INCLUDED
#define HILL__EXCEPTIONS_HPP_INCLUDED

#include <exception>

namespace hill {

	class internal_exception : public std::exception {
		const char *what() const noexcept override { return "Internal error"; }
	};

	class not_implemented_exception: public std::exception {
		const char *what() const noexcept override {return "Not implemented";}
	};

	class semantic_error_exception: public std::exception {
		const char *what() const noexcept override {return "Semantic error";}
	};
}

#endif /* HILL__EXCEPTIONS_HPP_INCLUDED */
