#ifndef HILL__EXCEPTIONS_HH_INCLUDED
#define HILL__EXCEPTIONS_HH_INCLUDED

#include <exception>
#include <string>

namespace hill {
	enum class error_code
	{
		NO_ERROR,
		CALLING_NO_FUNC,
		UNDEFINED_ID,
		ARRAY_ELM_TYPE_MISMATCH,
		MEMBER_ACCESS_ON_NO_TUPLE,
		UNKNOWN_MEMBER_NAME,
	};

	struct exception: std::exception {
		virtual error_code get_error_code()=0;
	};

	struct internal_exception : exception {
		const char *what() const noexcept override { return "Internal error"; }
		error_code get_error_code() {return (error_code)-1;}
	};

	struct not_implemented_exception: exception {
		const char *what() const noexcept override {return "Not implemented";}
		error_code get_error_code() {return (error_code)-1;}
	};

	struct semantic_error_exception: exception {
		semantic_error_exception(error_code code): code(code) {}

		const char *what() const noexcept override {return "Semantic error";}
		error_code get_error_code() {
			return code;
		};

		error_code code;
	};

	inline std::string error_code_to_str(error_code ec)
	{
		return std::to_string((int)ec);
	}
}

#endif /* HILL__EXCEPTIONS_HH_INCLUDED */
