#include "CameraControllImpl.h"
#include <D3DX10math.h>
D3DXMATRIX rotate;
CameraControllImpl::CameraControllImpl(Camera *c)
{
	this->camera = c;
	offset = 700;
	rot = 0;
	rotx = 0;
	RotateCamera(0, 0);
	Vec3 look(camera->GetLook().x, camera->GetLook().y, camera->GetLook().z);
}

CameraControllImpl::~CameraControllImpl(){}

void CameraControllImpl::Update()
{
	Vec3 pos = GetPosition();
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

void CameraControllImpl::Follow(Vec3 target)
{
	this->target = target;
}

void CameraControllImpl::SetOffset(float offset)
{
	this->offset = offset;
}

void CameraControllImpl::RotateCamera(float mouseX, float mouseY)
{
	o.Y = sin(rotx);
	o.X = cos(rot)*cos(rotx);
	o.Z = sin(rot)*cos(rotx);

	rot -= mouseX / 200;
	rotx -= mouseY / 200;

	if (rotx > 1)
		rotx = 1;
	else if (rotx < 0)
		rotx = 0;
}

void CameraControllImpl::Zoom(float d, float speed)
{
	if ((d < 0 && offset < MIN_ZOOM) || (d > 0 && offset > MAX_ZOOM))
		offset -= d * speed;
}

float Lerp(float a, float b, float amount)
{
	return 0;
}