#pragma once
#define NOMINMAX
#include <SimpleMath.h>

class Camera
{
public:
	Camera() { rotate(0, 0); };
	Camera(const Camera&) = default;
	Camera(Camera&&) = delete;
	Camera& operator=(const Camera&) = delete;
	Camera& operator=(Camera&&) = delete;
	~Camera() = default;

	[[nodiscard]] DirectX::SimpleMath::Matrix getProjection() const noexcept { return m_projection; }
	[[nodiscard]] DirectX::SimpleMath::Matrix getView() const noexcept { return m_view; }

	void setProjection(float fov, float aspectRatio, float nearZ, float farZ);

	void rotate(float dPitch, float dYaw);
	void move(float dX, float dY, float dZ);

private:
	DirectX::SimpleMath::Matrix m_projection{};
	DirectX::SimpleMath::Matrix m_view{};
	DirectX::SimpleMath::Vector3 m_position{ 0, 0, 0 };
	float m_pitch{ 0 };
	float m_yaw{ 0 };
};
