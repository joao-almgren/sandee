#pragma once
#include <SimpleMath.h>
using namespace DirectX::SimpleMath;

class Camera
{
public:
	Camera() { rotate(0, 0); };
	Camera(const Camera&) = default;
	Camera(Camera&&) = delete;
	Camera& operator=(const Camera&) = delete;
	Camera& operator=(Camera&&) = delete;
	~Camera() = default;

	Matrix getProjection() const { return m_projection; }
	Matrix getView() const { return m_view; }

	void setProjection(float fov, float aspectRatio, float nearZ, float farZ);
	void resetView();

	void rotate(float dPitch, float dYaw);
	void moveUp(float scale = 1);
	void moveRight(float scale = 1);
	void moveForward(float scale = 1);

private:
	Matrix m_view{}, m_projection{};
	float m_pitch{ 0 }, m_yaw{ 0 };
	Vector3 m_position{ 0, 0, 0 };
	Vector3 m_direction{ 0, 0, 1 }, m_right{ 1, 0, 0 }, m_up{ 0, 1, 0 };
};
