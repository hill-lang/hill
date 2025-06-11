#ifndef HILL__LSP__METHODS_TEXT_DOCUMENT__HOVER_HPP_INCLUDED
#define HILL__LSP__METHODS_TEXT_DOCUMENT__HOVER_HPP_INCLUDED

#include "../../models.hpp"
#include "../../logger.hpp"
#include "../../server_state.hpp"

namespace hill::lsp::methods {

	inline std::variant<std::optional<models::result_t>, models::response_error> text_document_hover(const models::request_message &req)
	{
		using namespace std::literals::string_literals;

		auto params = models::hover_params::from_json(req.params.value()).value();

		//params.text_document.uri

		auto res = models::hover {
			.contents = models::markup_content {
				.kind = models::markup_kind::MARKDOWN,
				.value =
R"(### This is a hover

```typescript
someCode();
```

Uri: ")" + params.text_document.uri
+ "\"\\\nLine: "s + std::to_string(params.position.line)
+ "\\\nChar: "s + std::to_string(params.position.character)
			},
		};
		return res.json();
	}
}

#endif /* HILL__LSP__METHODS_TEXT_DOCUMENT__HOVER_HPP_INCLUDED */
