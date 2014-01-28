#include "CameraControllImpl.h"
#include <DirectXMath.h>

using namespace DirectX;

XMMATRIX rotate;

template <typename T>
static T Lerp(T a, T b, float amount)
{
	return a + (b - a) * amount;
}
CameraControllImpl::CameraControllImpl(Camera *c)
{
	this->camera = c;
	offset = MIN_ZOOM;
	yaw = 0;
	pitch = 0;
	targetPitch = 0;
	targetYaw = 0;
	targetZoom = offset;
	Vec3 look(camera->GetLook().x, camera->GetLook().y, camera->GetLook().z);
}

CameraControllImpl::~CameraControllImpl(){}

void CameraControllImpl::Update(float dt)
{
	pitch = Lerp(pitch, targetPitch, dt*5);
	yaw = Lerp(yaw, targetYaw, dt*5);

	o.Y = sin(pitch);
	o.X = cos(yaw)*cos(pitch);
	o.Z = sin(yaw)*cos(pitch);
	o = o.Normalize();
	targetY = Lerp(targetY, target.Y, 2 * dt);
	
	target.Y = targetY;
	camera->LookAt(target);
	offset = Lerp(offset, targetZoom, 3 * dt);
	SetPosition(Vec3(target + (o*offset)));
}

void CameraControllImpl::SetDirection(Vec3 direction)
{
	this->camera->SetDirection(direction);
}

Vec3 CameraControllImpl::GetPosition()
{
	Vec3 pos;
	pos.X = camera->GetPosition().x;
	pos.Y = camera->GetPosition().y;
	pos.Z = camera->GetPosition().z;
	return pos;
}

void CameraControllImpl::SetPosition(Vec3 pos)
{
	camera->SetPosition(XMFLOAT3(pos.X, pos.Y, pos.Z));
}

Vec3 CameraControllImpl::GetDirection()
{
	return Vec3::Zero();
}

Vec3 CameraControllImpl::GetLook()
{
	return o*-1.0f;
}

Vec3 CameraControllImpl::GetRight()
{
	Vec3 right;
	right.X = camera->GetRight().x;
	right.Y = camera->GetRight().y;
	right.Z = camera->GetRight().z;
	return right;
}

Vec3 CameraControllImpl::GetUp()
{
	Vec3 up;
	up.X = camera->GetUp().x;
	up.Y = camera->GetUp().y;
	up.Z = camera->GetUp().z;
	return up;
}

float CameraControllImpl::GetYaw()
{
	return yaw;
}

float CameraControllImpl::GetPitch()
{
	return pitch;
}

void CameraControllImpl::Follow(Vec3 target)
{
	this->target = target;
	this->target.Y += 10; // Place the camera slightly over the player
}

void CameraControllImpl::SetOffset(float offset)
{
	this->offset = offset;
}

void CameraControllImpl::RotateCamera(float mouseX, float mouseY)
{
	if (targetYaw - yaw < 1 && targetYaw - yaw > -1)
	{
		targetYaw -= mouseX / 350;
		targetPitch -= mouseY / 250;
	}
	if (targetPitch > 1)
		targetPitch = 1;
	else if (targetPitch < -0.1)
		targetPitch = -0.1;  
}

void CameraControllImpl::Zoom(float d, float speed)
{
	if ((d < 0 && targetZoom < MIN_ZOOM) || (d > 0 && targetZoom > MAX_ZOOM))
		targetZoom -= d * speed;
}

