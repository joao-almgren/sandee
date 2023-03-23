#pragma once
#include <memory>

class Cube
{
public:
	explicit Cube(std::shared_ptr<class Graphics> pGraphics);
	Cube(const Cube&) = delete;
	Cube(Cube&&) = delete;
	Cube& operator=(const Cube&) = delete;
	Cube& operator=(Cube&&) = delete;
	~Cube();

	[[nodiscard]] bool load() const;
	void update(float tick) const;
	void draw(const class Camera & camera) const;

private:
	std::unique_ptr<class CubeImpl> m_pImpl{ nullptr };
};
