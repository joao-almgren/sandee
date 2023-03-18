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

	updateView();
}

void Camera::move(const float dX, const float dY, const float dZ)
{
	m_position += dZ * m_forward;
	m_position += dX * m_right;
	m_position += dY * m_up;

	updateView();
}

void Camera::updateView()
{
	m_forward.x = cosf(m_yaw) * cosf(m_pitch);
	m_forward.y = sinf(m_pitch);
	m_forward.z = sinf(m_yaw) * cosf(m_pitch);
	m_forward.Normalize();

	m_forward.Cross(Vector3::Up, m_right);
	m_right.Normalize();
	m_right.Cross(m_forward, m_up);
	m_up.Normalize();

	m_view = Matrix::CreateLookAt(m_position, m_position + m_forward, m_up);
}
