#ifndef HILL__UTILS__THREAD_POOL_HH_INCLUDED
#define HILL__UTILS__THREAD_POOL_HH_INCLUDED

#include <functional>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>

namespace hill::utils {

	struct thread_pool {
		thread_pool(): should_terminate(false) { }
		~thread_pool()
		{
			stop();
			join();
		}

		bool start()
		{
			if (threads.size() != 0) {
				return false;
			}

			should_terminate = false;

			const uint32_t thread_count = std::thread::hardware_concurrency();
			for (uint32_t i=0; i<thread_count; ++i) {
				threads.emplace_back(std::thread(&thread_pool::thread_loop, this));
			}

			return true;
		}

		void stop()
		{
			{
				std::unique_lock<std::mutex> lock(queue_mutex);
				should_terminate = true;
			}
			mutex_condition.notify_all();
		}

		bool join()
		{
			{
				std::unique_lock<std::mutex> lock(queue_mutex);
				if (!should_terminate) {
					return false;
				}
			}

			for (auto &t : threads) {
				t.join();
			}
			threads.clear();

			return true;
		}

		void queue_job(const std::function<void()> &job)
		{
			{
				std::unique_lock<std::mutex> lock(queue_mutex);
				jobs.push(job);
			}
			mutex_condition.notify_one();
		}

	private:
		std::vector<std::thread> threads;
		bool should_terminate;
		std::queue<std::function<void()>> jobs;
		std::mutex queue_mutex;
		std::condition_variable mutex_condition; // Allows threads to wait on new jobs or termination

	private:
		void thread_loop()
		{
			while (true) {
				std::function<void()> job;
				{
					std::unique_lock<std::mutex> lock(queue_mutex);
					mutex_condition.wait(lock, [this] {return !jobs.empty() || should_terminate;});
					if (should_terminate) {
						return;
					}
					job = jobs.front();
					jobs.pop();
				}
				try {
					job();
				} catch (const std::exception &) {}
			}
		}
	};
}

#endif /* HILL__UTILS__THREAD_POOL_HH_INCLUDED */
