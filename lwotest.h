#pragma once
#include <memory>

class LwoTest
{
public:
	explicit LwoTest(std::shared_ptr<class Graphics> pGraphics);
	LwoTest(const LwoTest&) = delete;
	LwoTest(LwoTest&&) = delete;
	LwoTest& operator=(const LwoTest&) = delete;
	LwoTest& operator=(LwoTest&&) = delete;
	~LwoTest();

	[[nodiscard]] bool load() const;
	void draw(const class Camera& camera) const;
	void update(float tick) const;

private:
	std::unique_ptr<class LwoTestImpl> m_pImpl{ nullptr };
};
