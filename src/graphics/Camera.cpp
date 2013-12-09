#include "Camera.h"


Camera::Camera(void)
	: mPosition(0.0f, 0.0f, 0.0f), 
	mRight(1.0f, 0.0f, 0.0f),
	mUp(0.0f, 1.0f, 0.0f),
	mLook(0.0f, 0.0f, 1.0f)
{
	SetLens(0.25f*MathHelper::pi, 1.0f, 1.0f, 1000.0f);
	Yaw = 0;
	Pitch = 0;
	Roll = 0;
}


Camera::~Camera(void)
{
}

XMVECTOR Camera::GetPositionXM() const
{
	return XMLoadFloat3(&mPosition);
}

XMFLOAT3 Camera::GetPosition() const
{
	return mPosition;
}

void Camera::SetPosition( float x, float y, float z )
{
	mPosition = XMFLOAT3(x, y, z);
}

void Camera::SetPosition( const XMFLOAT3& v )
{
	mPosition = v;
}

XMVECTOR Camera::GetRightXM() const
{
	return XMLoadFloat3(&mRight);
}

XMFLOAT3 Camera::GetRight() const
{
	return mRight;
}

XMVECTOR Camera::GetUpXM() const
{
	return XMLoadFloat3(&mUp);
}

XMFLOAT3 Camera::GetUp() const
{
	return mUp;
}

XMVECTOR Camera::GetLookXM() const
{
	return XMLoadFloat3(&mLook);
}

XMFLOAT3 Camera::GetLook() const
{
	return mLook;
}

XMMATRIX Camera::GetViewMatrix() const
{
	return XMLoadFloat4x4(&mView);
}

XMMATRIX Camera::GetProjMatrix() const
{
	return XMLoadFloat4x4(&mProj);
}

XMMATRIX Camera::GetViewProjMatrix() const
{
	return XMMatrixMultiply(GetViewMatrix(), GetProjMatrix());
}

#pragma region Frustum
float Camera::GetNearZ() const
{
	return mNearZ;
}

float Camera::GetFarZ() const
{
	return mFarZ;
}

float Camera::GetAspect() const
{
	return mAspect;
}

float Camera::GetFovY() const
{
	return mFovY;
}

float Camera::GetFovX() const
{
	float halfWidth = 0.5f*GetNearWindowWidth();
	return 2.0f*atan(halfWidth / mNearZ);
}

void Camera::SetLens( float fovY, float aspect, float zn, float zf )
{
	// Cache properties
	mFovY = fovY;
	mAspect = aspect;
	mNearZ = zn;
	mFarZ = zf;

	mNearWindowHeight = 2.0f * mNearZ * tanf( 0.5f*mFovY );
	mFarWindowHeight  = 2.0f * mFarZ * tanf( 0.5f*mFovY );

	XMMATRIX P = XMMatrixPerspectiveFovLH(mFovY, mAspect, mNearZ, mFarZ);
	XMStoreFloat4x4(&mProj, P);
}
#pragma endregion Frustum

float Camera::GetNearWindowWidth() const
{
	return mAspect * mNearWindowHeight;
}

float Camera::GetNearWindowHeight() const
{
	return mNearWindowHeight;
}

float Camera::GetFarWindowWidth() const
{
	return mAspect * mFarWindowHeight;
}

float Camera::GetFarWindowHeight() const
{
	return mFarWindowHeight;
}

void Camera::UpdateViewMatrix()
{
	XMVECTOR right = XMLoadFloat3(&mRight);
	XMVECTOR up = XMLoadFloat3(&mUp);
	XMVECTOR look = XMLoadFloat3(&mLook);
	XMVECTOR pos = XMLoadFloat3(&mPosition);

	// Orthonormalize right, up and look vectors
	look = XMVector3Normalize(look); // Unit length
	up = XMVector3Normalize(XMVector3Cross(look, right)); // Compute new corrected vector & normalize

	// Compute new corrected right vector (up & look already orthonormalized)
	right = XMVector3Cross(up, look);

	// Fill in the view matrix entries
	float x = -XMVectorGetX(XMVector3Dot(pos, right));
	float y = -XMVectorGetX(XMVector3Dot(pos, up));
	float z = -XMVectorGetX(XMVector3Dot(pos, look));

	XMStoreFloat3(&mRight, right);
	XMStoreFloat3(&mUp, up);
	XMStoreFloat3(&mLook, look);

	mView(0, 0) = mRight.x;
	mView(1, 0) = mRight.y;
	mView(2, 0) = mRight.z;
	mView(3, 0) = x;

	mView(0, 1) = mUp.x;
	mView(1, 1) = mUp.y;
	mView(2, 1) = mUp.z;
	mView(3, 1) = y;

	mView(0, 2) = mLook.x;
	mView(1, 2) = mLook.y;
	mView(2, 2) = mLook.z;
	mView(3, 2) = z;

	mView(0, 3) = 0.0f;
	mView(1, 3) = 0.0f;
	mView(2, 3) = 0.0f;
	mView(3, 3) = 1.0f;
}


XNA::Frustum Camera::GetFrustum() const
{
	return mFrustum;
}

void Camera::ComputeFrustum()
{
	ComputeFrustumFromProjection(&mFrustum, &GetProjMatrix());
}
