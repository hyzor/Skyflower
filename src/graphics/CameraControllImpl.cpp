#include "CameraControllImpl.h"

CameraControllImpl::CameraControllImpl(Camera *c)
{

}

CameraControllImpl::~CameraControllImpl(){}

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