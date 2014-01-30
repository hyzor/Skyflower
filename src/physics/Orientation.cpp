#include "Orientation.h"

Orientation::Orientation()
{
	//this->look = Vec3(0.0f, 0.0f, 1.0f);
	//this->right = Vec3(1.0f, 0.0f, 0.0f);
	//this->up = Vec3(0.0f, 1.0f, 0.0f);

	this->look = Vec3(0.0f, 0.0f, -1.0f);
	this->right = Vec3(-1.0f, 0.0f, 0.0f);
	this->up = Vec3(0.0f, 1.0f, 0.0f);
	this->rotRelativeCam = Vec3(0.0f, 0.0f, 0.0f);

	//this->look = Vec3(1.0f, 0.0f, 0.0f); //remember to correct walk/move to correspond to the right/up/look
	//this->right = Vec3(0.0f, 0.0f, -1.0f);
	//this->up = Vec3(0.0f, 1.0f, 0.0f);
}

Orientation::Orientation(const Orientation &other)
{
	this->look = other.look;
	this->right = other.right;
	this->up = other.up;
}

Orientation::~Orientation()
{
}

void Orientation::Update(float dt)
{
	Vec3 r = this->right;
	Vec3 u = this->up;
	Vec3 l = this->look;

	l.Normalize();
	Vec3 cross = l.Cross(r);
	cross.Normalize();
	u = cross;
	
	cross = u.Cross(l);
	r = cross;

	this->right = r;
	this->up = u;
	this->look = l;
}

void Orientation::Strafe(float length, Vec3 &pos)
{
	pos += this->right * length;
}

void Orientation::Walk(float length, Vec3 &pos)
{
	pos += this->look * length;
}

void Orientation::RotateX(Vec3 &rot, float angle)
{
	rot.X += angle;

	DirectX::XMMATRIX rotX;
	rotX = DirectX::XMMatrixRotationX(angle);
	DirectX::XMVECTOR u = DirectX::XMLoadFloat3(&(DirectX::XMFLOAT3(this->up.X, this->up.Y, this->up.Z)));
	DirectX::XMVECTOR l = DirectX::XMLoadFloat3(&(DirectX::XMFLOAT3(this->look.X, this->look.Y, this->look.Z)));

	DirectX::XMFLOAT3 rFloat3;

	u = DirectX::XMVector3TransformNormal(u, rotX);
	DirectX::XMStoreFloat3(&rFloat3, u);
	this->up = Vec3(rFloat3.x, rFloat3.y, rFloat3.z);

	l = DirectX::XMVector3TransformNormal(l, rotX);
	DirectX::XMStoreFloat3(&rFloat3, l);
	this->look = Vec3(rFloat3.x, rFloat3.y, rFloat3.z);
}

void Orientation::RotateY(Vec3 &rot, float angle)
{
	rot.Y += angle;
	//rot.Y = angle;

	DirectX::XMMATRIX mRotY;
	mRotY = DirectX::XMMatrixRotationY(angle);
	DirectX::XMVECTOR l = DirectX::XMLoadFloat3(&(DirectX::XMFLOAT3(this->look.X, this->look.Y, this->look.Z)));
	DirectX::XMVECTOR r = DirectX::XMLoadFloat3(&(DirectX::XMFLOAT3(this->right.X, this->right.Y, this->right.Z)));

	r = DirectX::XMVector3TransformNormal(r, mRotY);
	DirectX::XMFLOAT3 rFloat3;
	DirectX::XMStoreFloat3(&rFloat3, r);
	this->right = Vec3(rFloat3.x, rFloat3.y, rFloat3.z);

	l = DirectX::XMVector3TransformNormal(l, mRotY);
	DirectX::XMFLOAT3 lFloat3;
	DirectX::XMStoreFloat3(&lFloat3, l);
	this->look = Vec3(lFloat3.x, lFloat3.y, lFloat3.z);

}

void Orientation::RotateZ(Vec3 &rot, float angle)
{
	rot.Z += angle;

	DirectX::XMMATRIX mRotZ;
	mRotZ = DirectX::XMMatrixRotationZ(angle);
	DirectX::XMVECTOR u = DirectX::XMLoadFloat3(&(DirectX::XMFLOAT3(this->up.X, this->up.Y, this->up.Z)));
	DirectX::XMVECTOR r = DirectX::XMLoadFloat3(&(DirectX::XMFLOAT3(this->right.X, this->right.Y, this->right.Z)));

	r = DirectX::XMVector3TransformNormal(r, mRotZ);
	DirectX::XMFLOAT3 rFloat3;
	DirectX::XMStoreFloat3(&rFloat3, r);
	this->right = Vec3(rFloat3.x, rFloat3.y, rFloat3.z);

	u = DirectX::XMVector3TransformNormal(u, mRotZ);
	DirectX::XMFLOAT3 uFloat3;
	DirectX::XMStoreFloat3(&uFloat3, u);
	this->up = Vec3(uFloat3.x, uFloat3.y, uFloat3.z);
}



void Orientation::RotateAxis(Vec3 &rot, Vec3 axis, float angle)
{
	DirectX::XMMATRIX rotX;
	DirectX::XMVECTOR a = DirectX::XMLoadFloat3(&(DirectX::XMFLOAT3(axis.X, axis.Y, axis.Z)));
	rotX = DirectX::XMMatrixRotationAxis(a, angle);

	DirectX::XMVECTOR u = DirectX::XMLoadFloat3(&(DirectX::XMFLOAT3(this->up.X, this->up.Y, this->up.Z)));
	DirectX::XMVECTOR l = DirectX::XMLoadFloat3(&(DirectX::XMFLOAT3(this->look.X, this->look.Y, this->look.Z)));
	DirectX::XMVECTOR r = DirectX::XMLoadFloat3(&(DirectX::XMFLOAT3(this->right.X, this->right.Y, this->right.Z)));

	r = DirectX::XMVector3TransformNormal(r, rotX);
	DirectX::XMFLOAT3 rFloat3;
	DirectX::XMStoreFloat3(&rFloat3, r);
	this->right = Vec3(rFloat3.x, rFloat3.y, rFloat3.z);

	u = DirectX::XMVector3TransformNormal(u, rotX);
	DirectX::XMStoreFloat3(&rFloat3, u);
	this->up = Vec3(rFloat3.x, rFloat3.y, rFloat3.z);

	l = DirectX::XMVector3TransformNormal(l, rotX);
	DirectX::XMStoreFloat3(&rFloat3, l);
	this->look = Vec3(rFloat3.x, rFloat3.y, rFloat3.z);
}

void Orientation::ResetRotationXYZ(Vec3 &rot, Axis axis)
{
	rot = Vec3(0.0f, 0.0f, 0.0f);
	switch (axis)
	{
	case X:
		rot.X = 0.0f;
		break;
	case Y:
		rot.Y = 0.0f;
		break;
	case Z:
		rot.Z = 0.0f;
		break;
	}

	this->look = Vec3(1.0f, 0.0f, 0.0f);
	this->right = Vec3(0.0f, 0.0f, -1.0f);
	this->up = Vec3(0.0f, 1.0f, 0.0f);
}

void Orientation::SetOrientation(Vec3 look, Vec3 right, Vec3 up)
{
	this->look = look;
	this->right = right;
	this->up = up;
}

void Orientation::SetRotRelativeCam(Vec3 rot)
{
	this->rotRelativeCam = rot;
}

Vec3 Orientation::GetLook() const
{
	return this->look;
}

Vec3 Orientation::GetRight() const
{
	return this->right;
}

Vec3 Orientation::GetUp() const
{
	return this->up;
}

Vec3 Orientation::GetRotRelativeCam() const
{
	return this->rotRelativeCam;
}