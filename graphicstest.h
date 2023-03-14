#pragma once
#include <memory>
#include "graphics.h"
#include "camera.h"

class GraphicsTest
{
public:
	explicit GraphicsTest(std::shared_ptr<Graphics> pGraphics);
	GraphicsTest(const GraphicsTest&) = delete;
	GraphicsTest(GraphicsTest&&) = delete;
	GraphicsTest& operator=(const GraphicsTest&) = delete;
	GraphicsTest& operator=(GraphicsTest&&) = delete;
	~GraphicsTest();

	[[nodiscard]] bool load() const;
	void draw(const Camera & camera) const;
	void update(float tick) const;

private:
	std::unique_ptr<class GraphicsTestImpl> m_pImpl{ nullptr };
};
