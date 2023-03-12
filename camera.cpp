#include "camera.h"
using namespace DirectX;
using namespace DirectX::SimpleMath;

void Camera::setProjection(const float fov, const float aspectRatio, const float nearZ, const float farZ)
{
	m_projection = XMMatrixPerspectiveFovLH(fov, aspectRatio, nearZ, farZ);
}

void Camera::rotate(const float dPitch, const float dYaw)
{
	m_pitch += dPitch;
	m_yaw += dYaw;

	if (m_pitch > XM_PIDIV2)
		m_pitch = XM_PIDIV2;
	else if (m_pitch < -XM_PIDIV2)
		m_pitch = -XM_PIDIV2;

	const Matrix matY = Matrix::CreateRotationY(m_yaw);
	const Matrix matX = Matrix::CreateRotationX(m_pitch);
	const Matrix matRot = matY * matX;
	m_direction = matRot.Forward();

	const Vector3 at(m_position + m_direction);
	const Vector3 up(0, 1, 0);
	m_view = XMMatrixLookAtLH(m_position, at, up);

	m_right = m_view.Right();
	m_up = m_view.Up();
	m_direction = m_view.Forward();
}

void Camera::resetView()
{
	const Vector3 at(m_position + m_direction);
	const Vector3 up(0, 1, 0);
	m_view = XMMatrixLookAtLH(m_position, at, up);
}

void Camera::moveRight(const float scale)
{
	m_position += scale * m_right;
}

void Camera::moveForward(const float scale)
{
	m_position += scale * m_direction;
}

void Camera::moveUp(const float scale)
{
	m_position += scale * m_up;
}
