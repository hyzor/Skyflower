#ifndef CAMERA_H
#define CAMERA_H

#pragma comment (lib, "d3dx9.lib")

#include "d3dUtilities.h"
#include <D3DX10math.h>
#include "xnacollision.h"
#include "shared\Vec3.h"

class Camera
{
public:
	Camera(void);
	~Camera(void);

	// Camera position functions
	XMVECTOR GetPositionXM() const;
	XMFLOAT3 GetPosition() const;

	void SetPosition(float x, float y, float z);
	void SetPosition(const XMFLOAT3& v);
	void SetDirection(Vec3 direction);
	void Rotate(Vec3 rotation);

	// Get functions
	XMVECTOR GetRightXM() const;
	XMFLOAT3 GetRight() const;
	XMVECTOR GetUpXM() const;
	XMFLOAT3 GetUp() const;
	XMVECTOR GetLookXM() const;
	XMFLOAT3 GetLook() const;

	// Get frustum (lens) properties
	float GetNearZ() const;
	float GetFarZ() const;
	float GetAspect() const;
	float GetFovY() const;
	float GetFovX() const;

	// Set frustum (lens)
	void SetLens(float fovY, float aspect, float zn, float zf);

	// Get matrices
	XMMATRIX GetViewMatrix() const;
	XMMATRIX GetProjMatrix() const;
	XMMATRIX GetViewProjMatrix() const;

	// Get near and far plane dimensions in view space coordinates
	float GetNearWindowWidth() const;
	float GetNearWindowHeight() const;
	float GetFarWindowWidth() const;
	float GetFarWindowHeight() const;

	void UpdateViewMatrix();


	XNA::Frustum GetFrustum() const;

	void ComputeFrustum();
	
	void Yaw(float);
	void Pitch(float);

	void LookAt(Vec3 at);

private:
	// Coordinate system relative to world space
	XMFLOAT3 mPosition; // view space origin
	XMFLOAT3 mRight; // view space x-axis
	XMFLOAT3 mUp; // view space y-axis
	XMFLOAT3 mLook; // view space z-axis

	// Matrix cache
	XMFLOAT4X4 mView; // View matrix
	XMFLOAT4X4 mProj; // Projection matrix

	// Frustum cache
	float mNearZ;
	float mFarZ;
	float mAspect;
	float mFovY;
	float mNearWindowHeight;
	float mFarWindowHeight;

	XNA::Frustum mFrustum;

};

#endif