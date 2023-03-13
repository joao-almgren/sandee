#include "fps.h"
#include <chrono>

using namespace std::chrono;

class Timer
{
public:
	Timer() : start{ steady_clock::now() } {}
	Timer(const Timer&) = delete;
	Timer(Timer&&) = delete;
	Timer& operator=(const Timer&) = delete;
	Timer& operator=(Timer&&) = delete;
	~Timer() = default;

	[[nodiscard]] auto value() const
	{
		return duration_cast<microseconds>(steady_clock::now() - start).count();
	}

	void reset()
	{
		start = steady_clock::now();
	}

private:
	steady_clock::time_point start;
};

FpsCounter::FpsCounter()
	: fps{ 60 }
	, averageFps{ 60 }
	, frameCount{ 0 }
	, timer{ std::make_unique<Timer>() }
{
}

FpsCounter::~FpsCounter() = default;

void FpsCounter::tick()
{
	frameCount++;

	if (timer->value() >= 1000000)
	{
		fps = frameCount;

		const double alpha = 0.25; // TODO: consider making this configurable
		averageFps = alpha * averageFps + (1.0 - alpha) * frameCount;

		frameCount = 0;
		timer->reset();
	}
}
