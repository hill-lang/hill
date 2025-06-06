#ifndef HILL__LSP__DOCUMENT_STORE_HPP_INCLUDED
#define HILL__LSP__DOCUMENT_STORE_HPP_INCLUDED

#include <filesystem>
#include <string>
#include <unordered_map>
#include <mutex>
#include <optional>

namespace hill::lsp {

	struct document_store {
		document_store() = default;

		bool exists(const std::filesystem::path &fpath)
		{
			std::lock_guard<std::mutex> guard(access_mutex);
			return store.contains(fpath);
		}

		std::optional<std::string> get(const std::filesystem::path &fpath)
		{
			std::lock_guard<std::mutex> guard(access_mutex);
			if (!store.contains(fpath)) return std::nullopt;
			else return store.at(fpath);
		}

		void set(const std::filesystem::path &fpath, const std::string &content)
		{
			std::lock_guard<std::mutex> guard(access_mutex);
			store[fpath] = content;
		}

		bool remove(const std::filesystem::path &fpath)
		{
			std::lock_guard<std::mutex> guard(access_mutex);
			if (!store.contains(fpath)) return false;

			store.erase(fpath);
			return true;
		}

	private:
		std::mutex access_mutex;
		std::unordered_map<std::filesystem::path, std::string> store;
	};
}

#endif /* HILL__LSP__DOCUMENT_STORE_HPP_INCLUDED */
