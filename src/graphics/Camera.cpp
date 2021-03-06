#include "Camera.h"

// Must be included last!
#include "shared/debug.h"

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


DirectX::BoundingFrustum Camera::GetFrustum() const
{
	return mFrustum;
}

void Camera::ComputeFrustum()
{
	//DirectX::ComputeFrustumFromProjection(&mFrustum, &GetProjMatrix());

	mFrustum = DirectX::BoundingFrustum(GetProjMatrix());
}

void Camera::LookAt(Vec3 at)
{
	XMVECTOR lookat = XMLoadFloat3(&XMFLOAT3(at.X, at.Y, at.Z));
	XMVECTOR up = XMLoadFloat3(&XMFLOAT3(0, 1, 0));
	XMVECTOR position = XMLoadFloat3(&XMFLOAT3(mPosition.x, mPosition.y, mPosition.z));

	XMMATRIX view = XMMatrixLookAtLH(position, lookat, up);// D3DXToRadian(degrees));
	XMStoreFloat4x4(&mView, view);

	mLook = XMFLOAT3(at.X - mPosition.x, at.Y - mPosition.y, at.Z - mPosition.z);
	mRight = XMFLOAT3(mView._11, mView._21, mView._31);
	mUp = XMFLOAT3(mView._12, mView._22, mView._32);
}

void Camera::UpdateOrthoMatrix(float screenWidth, float screenHeight, float zn, float zf)
{
	XMMATRIX orthographicProj = XMMatrixOrthographicLH(screenWidth, screenHeight, zn, zf);
	XMStoreFloat4x4(&mOrthographicProj, orthographicProj);
}

DirectX::XMMATRIX Camera::GetOrthoMatrix() const
{
	return XMLoadFloat4x4(&mOrthographicProj);
}

void Camera::UpdateBaseViewMatrix()
{
	/*
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

	mBaseView(0, 0) = mRight.x;
	mBaseView(1, 0) = mRight.y;
	mBaseView(2, 0) = mRight.z;
	mBaseView(3, 0) = x;

	mBaseView(0, 1) = mUp.x;
	mBaseView(1, 1) = mUp.y;
	mBaseView(2, 1) = mUp.z;
	mBaseView(3, 1) = y;

	mBaseView(0, 2) = mLook.x;
	mBaseView(1, 2) = mLook.y;
	mBaseView(2, 2) = mLook.z;
	mBaseView(3, 2) = z;

	mBaseView(0, 3) = 0.0f;
	mBaseView(1, 3) = 0.0f;
	mBaseView(2, 3) = 0.0f;
	mBaseView(3, 3) = 1.0f;
	*/

	//XMFLOAT3 right(1.0f, 0.0f, 0.0f);
	//XMFLOAT3 dir(0, 0, 1);
	XMVECTOR lookat = XMLoadFloat3(&XMFLOAT3(0.0f, 0.0f, 1.0f));
	XMVECTOR up = XMLoadFloat3(&XMFLOAT3(0.0f, 1.0f, 0.0f));
	XMVECTOR position = XMLoadFloat3(&XMFLOAT3(mPosition.x, mPosition.y, mPosition.z));

	XMMATRIX view = XMMatrixLookAtLH(position, lookat, up);// D3DXToRadian(degrees));
	XMStoreFloat4x4(&mBaseView, view);
}

DirectX::XMMATRIX Camera::GetBaseViewMatrix() const
{
	return XMLoadFloat4x4(&mBaseView);
}

void Camera::Update()
{
	XMVECTOR pos = XMLoadFloat3(&mPosition);
	XMMATRIX offset = XMMatrixTranslationFromVector(pos);
	XMMATRIX mrot = XMMatrixRotationX(0.0f);
	mrot *= XMMatrixRotationY(0.0f);
	mrot *= XMMatrixRotationZ(0.0f);
	XMMATRIX mscale = XMMatrixScaling(1.0f, 1.0f, 1.0f);

	XMMATRIX world = mscale*mrot*offset;

	XMStoreFloat4x4(&mWorld, world);
}

DirectX::XMMATRIX Camera::GetWorldMatrix() const
{
	return XMLoadFloat4x4(&mWorld);
}

void Camera::Rotate(float yaw, float pitch)
{
	XMVECTOR DefaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
	XMVECTOR DefaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR camTarget;
	XMVECTOR camRight;
	XMVECTOR camForward;
	
	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, 0);

	camTarget = XMVector3TransformCoord(DefaultForward, rotation);
	camTarget = XMVector3Normalize(camTarget);

	camRight = XMVector3TransformCoord(DefaultRight, rotation);
	camForward = XMVector3TransformCoord(DefaultForward, rotation);

	XMStoreFloat3(&mUp, XMVector3Cross(camForward, camRight));
	XMStoreFloat3(&mRight, camRight);
	XMStoreFloat3(&mLook, camForward);

	camTarget = XMLoadFloat3(&mPosition) + camTarget;

	view = XMMatrixLookAtLH(XMLoadFloat3(&mPosition), camTarget, XMLoadFloat3(&mUp));

	XMStoreFloat4x4(&mView, view);
}

void Camera::SetPrevViewProj(XMMATRIX& prevViewProj)
{
	XMStoreFloat4x4(&mPrevViewProj, prevViewProj);
}

DirectX::XMMATRIX Camera::GetPreviousViewProj() const
{
	return XMLoadFloat4x4(&mPrevViewProj);
}
