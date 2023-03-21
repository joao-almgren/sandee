#include "fps.h"
#include "timer.h"

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
