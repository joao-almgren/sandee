#pragma once
#include <memory>

class Skybox
{
public:
	Skybox(std::shared_ptr<class Graphics> pGraphics);
	Skybox(const Skybox&) = delete;
	Skybox(Skybox&&) = delete;
	Skybox& operator=(const Skybox&) = delete;
	Skybox& operator=(Skybox&&) = delete;
	~Skybox();

	[[nodiscard]] bool load() const;
	void update(float tick) const;
	void draw(const class Camera& camera) const;

private:
	std::unique_ptr<class SkyboxImpl> m_pImpl{ nullptr };
};
