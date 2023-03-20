#pragma once
#define NOMINMAX
#include <SimpleMath.h>

class Camera
{
public:
	Camera() { updateView(); };
	Camera(const Camera&) = default;
	Camera(Camera&&) = delete;
	Camera& operator=(const Camera&) = delete;
	Camera& operator=(Camera&&) = delete;
	~Camera() = default;

	[[nodiscard]] DirectX::SimpleMath::Matrix getProjection() const noexcept { return m_projection; }
	[[nodiscard]] DirectX::SimpleMath::Matrix getView() const noexcept { return m_view; }
	[[nodiscard]] DirectX::SimpleMath::Vector3 getPosition() const noexcept { return m_position; }

	void setProjection(float fov, float aspectRatio, float nearZ, float farZ);

	void rotate(float dPitch, float dYaw);
	void move(float dX, float dY, float dZ);

private:
	void updateView();

	DirectX::SimpleMath::Matrix m_projection{};
	DirectX::SimpleMath::Matrix m_view{};
	DirectX::SimpleMath::Vector3 m_position{ DirectX::SimpleMath::Vector3::Zero };
	DirectX::SimpleMath::Vector3 m_forward{ DirectX::SimpleMath::Vector3::Forward };
	DirectX::SimpleMath::Vector3 m_right{ DirectX::SimpleMath::Vector3::Right };
	DirectX::SimpleMath::Vector3 m_up{ DirectX::SimpleMath::Vector3::Up };
	float m_pitch{ 0 };
	float m_yaw{ DirectX::XM_PI };
};
