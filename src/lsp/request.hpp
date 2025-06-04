#ifndef HILL__LSP__REQUEST_HPP_INCLUDED
#define HILL__LSP__REQUEST_HPP_INCLUDED

namespace hill::lsp {

	enum class method {
		INITIALIZE,
	};

	struct request {
		request(method metod): has_id(false), id(0), metod(metod) {}
		request(size_t id, method metod): has_id(true), id(id), metod(metod) {}

		bool has_id;
		size_t id;
		method metod;
	};
}

#endif /* HILL__LSP__REQUEST_HPP_INCLUDED */
