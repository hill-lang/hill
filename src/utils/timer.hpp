#ifndef HILL__UTILS__TIMER_HPP_INCLUDED
#define HILL__UTILS__TIMER_HPP_INCLUDED

#include <chrono>

namespace hill::utils {

	struct timer
	{
		timer()
		{
			reset();
		}

		void reset()
		{
			this->start = std::chrono::high_resolution_clock::now();
		}

		double elapsed_sec() const
		{
			return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - this->start)
				.count() * 0.001 * 0.001 * 0.001;
		}

		double elapsed_ms() const
		{
			return elapsed_sec() * 1000.0;
		}

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> start;
	};
}

#endif /* HILL__UTILS__TIMER_HPP_INCLUDED */
