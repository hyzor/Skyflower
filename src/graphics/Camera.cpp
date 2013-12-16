#include "Camera.h"


Camera::Camera(void)
	: mPosition(0.0f, 0.0f, 0.0f), 
	mRight(1.0f, 0.0f, 0.0f),
	mUp(0.0f, 1.0f, 0.0f),
	mLook(0.0f, 0.0f, 1.0f)
{
	SetLens(0.25f*MathHelper::pi, 1.0f, 1.0f, 1000.0f);
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
void Camera::SetDirection(Vec3 direction)
{
	mLook.x = direction.X;
	mLook.y = direction.Y;
	mLook.z = direction.Z;
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
	return XMFLOAT3(mView(0, 0), mView(0, 1), mView(0, 2));
}

XMVECTOR Camera::GetUpXM() const
{
	return XMLoadFloat3(&mUp);
}

XMFLOAT3 Camera::GetUp() const
{
	return XMFLOAT3(mView(1, 0), mView(1, 1), mView(1, 2));
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
	/*XMVECTOR right = XMLoadFloat3(&mRight);
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
	mView(3, 3) = 1.0f;*/
}


XNA::Frustum Camera::GetFrustum() const
{
	return mFrustum;
}

void Camera::ComputeFrustum()
{
	ComputeFrustumFromProjection(&mFrustum, &GetProjMatrix());
}

void Camera::Yaw(float degrees)
{
	D3DXVECTOR3 up(0, 1, 0);
	D3DXVECTOR3 dir(mLook.x, mLook.y, mLook.z);
	D3DXVECTOR3 right(mRight.x, mRight.y, mRight.z);

	D3DXMATRIX yaw; // create a matrix to hold the rotation
	D3DXMatrixRotationAxis(&yaw, &up, (float)D3DXToRadian(degrees));

	// Update the view vectors of both the target and camera based on the rotation represented in our yaw matrix
	D3DXVec3TransformCoord(&dir, &dir, &yaw);
	//D3DXVec3TransformCoord(&tarView, &tarView, &yaw);

	// Also update the right vectors of the target and camera
	D3DXVec3TransformCoord(&right, &right, &yaw);
	//D3DXVec3TransformCoord(&tarRight, &tarRight, &yaw);

	//mUp = XMFLOAT3(dir.x, dir.y, dir.z);
	mRight = XMFLOAT3(right.x, right.y, right.z);
	mLook = XMFLOAT3(dir.x, dir.y, dir.z);
}

void Camera::Pitch(float degrees)
{
	int i = 5;
}

void Camera::LookAt(Vec3 at)
{
	D3DXVECTOR3 right(1, 0, 0);
	D3DXVECTOR3 dir(0, 0, 1);
	XMVECTOR lookat = XMLoadFloat3(&XMFLOAT3(at.X, at.Y, at.Z));
	XMVECTOR up = XMLoadFloat3(&XMFLOAT3(0, 1, 0));
	XMVECTOR position = XMLoadFloat3(&XMFLOAT3(mPosition.x, mPosition.y, mPosition.z));

	XMMATRIX view = XMMatrixLookAtLH(position, lookat, up);// D3DXToRadian(degrees));
	XMStoreFloat4x4(&mView, view);

	mLook = XMFLOAT3(at.X - mPosition.x, at.Y - mPosition.y, at.Z - mPosition.z);
	// Update the view vectors of both the target and camera based on the rotation represented in our yaw matrix
	/*D3DXVec3TransformCoord(&dir, &dir, &yaw);
	//D3DXVec3TransformCoord(&tarView, &tarView, &yaw);

	// Also update the right vectors of the target and camera
	D3DXVec3TransformCoord(&right, &right, &yaw);
	//D3DXVec3TransformCoord(&tarRight, &tarRight, &yaw);

	D3DXVec3TransformCoord(&up, &up, &yaw);

	mUp = XMFLOAT3(up.x, up.y, up.z);
	mRight = XMFLOAT3(right.x, right.y, right.z);
	mLook = XMFLOAT3(dir.x, dir.y, dir.z);*/
}