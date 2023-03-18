#pragma once
#include <memory>

class GraphicsTest
{
public:
	explicit GraphicsTest(std::shared_ptr<class Graphics> pGraphics);
	GraphicsTest(const GraphicsTest&) = delete;
	GraphicsTest(GraphicsTest&&) = delete;
	GraphicsTest& operator=(const GraphicsTest&) = delete;
	GraphicsTest& operator=(GraphicsTest&&) = delete;
	~GraphicsTest();

	[[nodiscard]] bool load() const;
	void update(float tick) const;
	void draw(const class Camera & camera) const;

private:
	std::unique_ptr<class GraphicsTestImpl> m_pImpl{ nullptr };
};
