#include "camera.h"
using namespace DirectX;
using namespace DirectX::SimpleMath;

void Camera::setProjection(const float fov, const float aspect, const float nearZ, const float farZ)
{
	mProjection = Matrix::CreatePerspectiveFieldOfView(fov, aspect, nearZ, farZ);
}

void Camera::rotate(const float dPitch, const float dYaw)
{
	mPitch += dPitch;
	mYaw += dYaw;

	if (mPitch > XM_PIDIV2)
		mPitch = XM_PIDIV2;
	else if (mPitch < -XM_PIDIV2)
		mPitch = -XM_PIDIV2;

	const Matrix matY = Matrix::CreateRotationY(mYaw);
	const Matrix matX = Matrix::CreateRotationX(mPitch);
	const Matrix matRot = matY * matX;
	mDir = matRot.Forward();

	const Vector3 at(mPos + mDir);
	const Vector3 yup(0, 1, 0);
	mView = Matrix::CreateLookAt(mPos, at, yup);

	mRight = mView.Right();
	mUp = mView.Up();
	mDir = mView.Forward();
}

void Camera::resetView()
{
	const Vector3 at(mPos + mDir);
	const Vector3 yup(0, 1, 0);
	mView = Matrix::CreateLookAt(mPos, at, yup);
}

void Camera::moveRight(const float scale)
{
	mPos += scale * mRight;
}

void Camera::moveForward(const float scale)
{
	mPos += scale * mDir;
}

void Camera::moveUp(const float scale)
{
	mPos += scale * mUp;
}
