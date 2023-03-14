#pragma once
#include <memory>

class FpsCounter
{
public:
	FpsCounter();
	FpsCounter(const FpsCounter&) = delete;
	FpsCounter(FpsCounter&&) = delete;
	FpsCounter& operator=(const FpsCounter&) = delete;
	FpsCounter& operator=(FpsCounter&&) = delete;
	~FpsCounter();

	void tick();
	[[nodiscard]] double getFps() const noexcept { return m_fps; }
	[[nodiscard]] double getAverageFps() const noexcept { return m_avgFps; }
	[[nodiscard]] unsigned int getFrameCount() const noexcept { return m_frameCount; }

private:
	double m_fps;
	double m_avgFps;
	unsigned int m_frameCount;
	std::unique_ptr<class Timer> m_timer;
};
