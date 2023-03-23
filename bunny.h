#pragma once
#include <memory>

class Bunny
{
public:
	explicit Bunny(std::shared_ptr<class Graphics> pGraphics);
	Bunny(const Bunny&) = delete;
	Bunny(Bunny&&) = delete;
	Bunny& operator=(const Bunny&) = delete;
	Bunny& operator=(Bunny&&) = delete;
	~Bunny();

	[[nodiscard]] bool load() const;
	void draw(const class Camera& camera) const;
	void update(float tick) const;

private:
	std::unique_ptr<class BunnyImpl> m_pImpl{ nullptr };
};
