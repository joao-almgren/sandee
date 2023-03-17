#include "camera.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

void Camera::setProjection(const float fov, const float aspectRatio, const float nearZ, const float farZ)
{
	m_projection = Matrix::CreatePerspectiveFieldOfView(fov, aspectRatio, nearZ, farZ);
}

void Camera::rotate(const float dPitch, const float dYaw)
{
	m_pitch += dPitch;
	const float pitchLimit = XM_PIDIV2 - 0.01f;
	m_pitch = std::max(-pitchLimit, m_pitch);
	m_pitch = std::min(+pitchLimit, m_pitch);

	m_yaw += dYaw;
	if (m_yaw > XM_2PI)
		m_yaw -= XM_2PI;
	if (m_yaw < 0)
		m_yaw += XM_2PI;

	const float y = sinf(m_pitch);
	const float r = cosf(m_pitch);
	const float z = r * cosf(m_yaw);
	const float x = r * sinf(m_yaw);

	const Vector3 target = m_position + Vector3(x, y, z);
	m_view = Matrix::CreateLookAt(m_position, target, Vector3::Up);
}

void Camera::move(const float dX, const float dY, const float dZ)
{
	const Quaternion q = Quaternion::CreateFromYawPitchRoll(m_yaw, m_pitch, 0);
	const Vector3 move = Vector3::Transform(Vector3(dX, dY, dZ), q);
	m_position += move;
	rotate(0, 0);
}
