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
	: m_fps{ 60 }
	, m_avgFps{ 60 }
	, m_frameCount{ 0 }
	, m_timer{ std::make_unique<Timer>() }
{
}

FpsCounter::~FpsCounter() = default;

void FpsCounter::tick()
{
	m_frameCount++;

	if (m_timer->value() >= 1000000)
	{
		m_fps = m_frameCount;

		const double alpha = 0.25; // TODO: consider making this configurable
		m_avgFps = alpha * m_avgFps + (1.0 - alpha) * m_frameCount;

		m_frameCount = 0;
		m_timer->reset();
	}
}
