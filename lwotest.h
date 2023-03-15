#pragma once
#include <memory>
#include "graphics.h"
#include "camera.h"

class LwoTest
{
public:
	explicit LwoTest(std::shared_ptr<Graphics> pGraphics);
	LwoTest(const LwoTest&) = delete;
	LwoTest(LwoTest&&) = delete;
	LwoTest& operator=(const LwoTest&) = delete;
	LwoTest& operator=(LwoTest&&) = delete;
	~LwoTest();

	[[nodiscard]] bool load() const;
	void draw(const Camera& camera) const;
	void update(float tick) const;

private:
	std::unique_ptr<class LwoTestImpl> m_pImpl{ nullptr };
};
