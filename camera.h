#pragma once
#include <SimpleMath.h>
using namespace DirectX::SimpleMath;

class Camera
{
public:
	Camera() { rotate(0, 0); };
	Camera(const Camera& g) = default;
	Camera(Camera&& g) = delete;
	Camera& operator=(const Camera& g) = delete;
	Camera& operator=(Camera&& g) = delete;
	~Camera() = default;

	Matrix getProjection() const { return mProjection; }
	Matrix getView() const { return mView; }

	void setProjection(float fov, float aspect, float nearZ, float farZ);
	void resetView();

	void rotate(float dPitch, float dYaw);
	void moveUp(float scale = 1);
	void moveRight(float scale = 1);
	void moveForward(float scale = 1);

private:
	Matrix mView{}, mProjection{};
	float mPitch{ 0 }, mYaw{ 0 };
	Vector3 mPos{ 0 , 0, 0 };
	Vector3 mDir{ 0, 0, 1 }, mRight{ 1, 0, 0 }, mUp{ 0, 1, 0 };
};
