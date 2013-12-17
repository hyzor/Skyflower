#include "CameraControllImpl.h"
#include <D3DX10math.h>
D3DXMATRIX rotate;
static float Lerp(float a, float b, float amount)
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
	//RotateCamera(0, 0);
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

void CameraControllImpl::Rotate(Vec3 rot)
{}

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
	//Vec3 look;
	//look.X = camera->GetLook().x;
	//look.Y = camera->GetLook().y;
	//look.Z = camera->GetLook().z;
	//return look.Normalize();
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

void CameraControllImpl::Follow(Vec3 target)
{
	this->target = target;
	this->target.Y += 10;
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
		targetPitch -= mouseY / 200;
	}
	if (targetPitch > 1)
		targetPitch = 1;
	else if (targetPitch < 0)
		targetPitch = 0;
}

void CameraControllImpl::Zoom(float d, float speed)
{
	if ((d < 0 && offset < MIN_ZOOM) || (d > 0 && offset > MAX_ZOOM))
		offset -= d * speed;
}

