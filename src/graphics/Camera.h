#ifndef CAMERA_H
#define CAMERA_H

#include "d3dUtilities.h"
#include <DirectXCollision.h>
#include <DirectXMath.h>
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
	void UpdateOrthoMatrix(float screenWidth, float screenHeight, float zn, float zf);

	// Get matrices
	XMMATRIX GetViewMatrix() const;
	XMMATRIX GetProjMatrix() const;
	XMMATRIX GetViewProjMatrix() const;
	XMMATRIX GetOrthoMatrix() const;
	XMMATRIX GetBaseViewMatrix() const;

	// Get near and far plane dimensions in view space coordinates
	float GetNearWindowWidth() const;
	float GetNearWindowHeight() const;
	float GetFarWindowWidth() const;
	float GetFarWindowHeight() const;

	//void UpdateViewMatrix();
	void UpdateBaseViewMatrix();


	DirectX::BoundingFrustum GetFrustum() const;

	void ComputeFrustum();
	
	void Pitch(float);

	void LookAt(Vec3 at);
	//float Yaw;
	//float Pitch;
	//float Roll;

private:
	// Coordinate system relative to world space
	XMFLOAT3 mPosition; // view space origin
	XMFLOAT3 mRight; // view space x-axis
	XMFLOAT3 mUp; // view space y-axis
	XMFLOAT3 mLook; // view space z-axis

	// Matrix cache
	XMFLOAT4X4 mView; // View matrix
	XMFLOAT4X4 mProj; // Projection matrix
	XMFLOAT4X4 mOrthographicProj; // 2D Projection matrix
	XMFLOAT4X4 mBaseView; // Base view matrix

	// Frustum cache
	float mNearZ;
	float mFarZ;
	float mAspect;
	float mFovY;
	float mNearWindowHeight;
	float mFarWindowHeight;

	DirectX::BoundingFrustum mFrustum;

};

#endif