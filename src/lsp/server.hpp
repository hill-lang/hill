#ifndef HILL__LSP__SERVER_HPP_INCLUDED
#define HILL__LSP__SERVER_HPP_INCLUDED

#include <iostream>
#include <string>

namespace hill::lsp {

	struct server {
		server() = default;

		bool running = true;

		void run()
		{
			std::string buf;
			buf.reserve(1024);

			//fread()

			while (running) {
				buf.push_back(std::cin.get());
				std::cout << "ECHO: " << buf << '\n';
			}
		}
	};
}

#endif /* HILL__LSP__SERVER_HPP_INCLUDED */
