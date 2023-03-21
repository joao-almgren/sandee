#pragma once
#include <chrono>

class Timer
{
public:
	Timer() { reset(); }
	Timer(const Timer&) = delete;
	Timer(Timer&&) = delete;
	Timer& operator=(const Timer&) = delete;
	Timer& operator=(Timer&&) = delete;
	~Timer() = default;

	[[nodiscard]] auto value() const
	{
		return duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count();
	}

	void reset()
	{
		start = std::chrono::steady_clock::now();
	}

private:
	std::chrono::steady_clock::time_point start;
};
