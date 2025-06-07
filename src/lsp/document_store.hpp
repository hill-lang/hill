#ifndef HILL__LSP__DOCUMENT_STORE_HPP_INCLUDED
#define HILL__LSP__DOCUMENT_STORE_HPP_INCLUDED

#include <string>
#include <unordered_map>
#include <mutex>
#include <optional>

namespace hill::lsp {

	struct document_store {
		document_store() = default;

		/*bool contains(const std::string &fpath)
		{
			std::lock_guard<std::mutex> guard(access_mutex);
			return store.contains(fpath);
		}*/

		std::optional<std::string> get(const std::string &fpath)
		{
			std::lock_guard<std::mutex> guard(access_mutex);
			if (!store.contains(fpath)) return std::nullopt;
			else return store.at(fpath);
		}

		void set(const std::string &fpath, const std::string &content)
		{
			std::lock_guard<std::mutex> guard(access_mutex);
			store[fpath] = content;
		}

		bool remove(const std::string &fpath)
		{
			std::lock_guard<std::mutex> guard(access_mutex);
			if (!store.contains(fpath)) return false;

			store.erase(fpath);
			return true;
		}

	private:
		std::mutex access_mutex;
		std::unordered_map<std::string, std::string> store;
	};
}

#endif /* HILL__LSP__DOCUMENT_STORE_HPP_INCLUDED */
