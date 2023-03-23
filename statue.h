#pragma once
#include <memory>

class Statue
{
public:
	explicit Statue(std::shared_ptr<class Graphics> pGraphics);
	Statue(const Statue&) = delete;
	Statue(Statue&&) = delete;
	Statue& operator=(const Statue&) = delete;
	Statue& operator=(Statue&&) = delete;
	~Statue();

	[[nodiscard]] bool load() const;
	void draw(const class Camera& camera) const;
	void update(float tick) const;

private:
	std::unique_ptr<class StatueImpl> m_pImpl{ nullptr };
};
