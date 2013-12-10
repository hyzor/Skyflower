#include "CameraControllImpl.h"
#include <D3DX10math.h>
D3DXMATRIX rotate;
CameraControllImpl::CameraControllImpl(Camera *c)
{
	this->camera = c;
	offset = 700;
	rot = 0;
	rotx = 0;

	Vec3 look(camera->GetLook().x, camera->GetLook().y, camera->GetLook().z);
}

CameraControllImpl::~CameraControllImpl(){}

void CameraControllImpl::Update(float dt,int mouseX, int mouseY)
{
	
	//camera->Yaw(0.01);
	Vec3 look(camera->GetLook().x, camera->GetLook().y, camera->GetLook().z);
	
	Vec3 deltaMouse = Vec3(mouseX, mouseY)-lastMousePos;

	Vec3 o;// = (look*this->offset);
	o.Y = sin(rotx);
	o.X = cos(rot)*cos(rotx);
	o.Z = sin(rot)*cos(rotx);
	rot += deltaMouse.X/1000;
	rotx += deltaMouse.Y/ 1000;

	SetPosition(Vec3(target + (o*offset)));

	//SetPosition(GetPosition() + Vec3(0.0f, 0.05f, 0.0f));
	camera->LookAt(target);
	//lookat.Y += 0.1f;

	lastMousePos = Vec3(mouseX, mouseY);
	
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

