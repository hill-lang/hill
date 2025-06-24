#ifndef HILL__LSP__SERVER_STATE_HH_INCLUDED
#define HILL__LSP__SERVER_STATE_HH_INCLUDED

#include "document_store.hh"
#include "models.hh"

#include <atomic>
#include <set>
#include <mutex>

namespace hill::lsp {

	struct request_state {
		void on_recv(int req_id)
		{
			std::lock_guard<std::mutex> guard(mutex);
			in_flight_requests.insert(req_id);
		}

		void on_complete(int req_id)
		{
			std::lock_guard<std::mutex> guard(mutex);
			in_flight_requests.erase(req_id);
			if (cancel_request_ids.contains(req_id)) {
				cancel_request_ids.erase(req_id);
			}
		}

		bool on_cancel(int req_id)
		{
			std::lock_guard<std::mutex> guard(mutex);
			if (in_flight_requests.contains(req_id)) {
				cancel_request_ids.insert(req_id);
				return true;
			} else {
				return false;
			}
		}

		bool cancelled(int req_id)
		{
			std::lock_guard<std::mutex> guard(mutex);
			return cancel_request_ids.contains(req_id);
		}

	private:
		std::mutex mutex;
		std::set<int> in_flight_requests;
		std::set<int> cancel_request_ids;
	};

	struct server_state {
		std::atomic<bool> running = true;
		std::atomic<bool> initialized = false;

		lsp::document_store document_store;
		lsp::request_state request_state;

		const models::server_capabilities server_capabilities = {
			.position_encoding = models::position_encoding_kind::UTF16,
			.text_document_sync = models::text_document_sync_kind::FULL,
			.completion_provider = models::completion_options{},
			.hover_provider = true,
			.document_formatting_provider = true,
		};

		static server_state &get()
		{
			static server_state state;
			return state;
		}
	};
}

#endif /* HILL__LSP__SERVER_STATE_HH_INCLUDED */
