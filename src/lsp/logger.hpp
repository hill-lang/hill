#ifndef HILL__LSP__LOGGER_HPP_INCLUDED
#define HILL__LSP__LOGGER_HPP_INCLUDED

#include <filesystem>
#include <fstream>

namespace hill::lsp {

	struct logger {
		std::ofstream ofs;

		void open(const std::filesystem::path &fpath)
		{
			std::filesystem::create_directories(fpath.parent_path());
			ofs.open(fpath, std::ios::binary);
		}
	};
}

#endif /* HILL__LSP__LOGGER_HPP_INCLUDED */
