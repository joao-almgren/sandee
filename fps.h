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
	[[nodiscard]] double getFps() const noexcept { return fps; }
	[[nodiscard]] double getAverageFps() const noexcept { return averageFps; }
	[[nodiscard]] unsigned int getFrameCount() const noexcept { return frameCount; }

private:
	double fps, averageFps;
	unsigned int frameCount;
	std::unique_ptr<class Timer> timer;
};
