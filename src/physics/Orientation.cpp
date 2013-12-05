#include "Orientation.h"

Orientation::Orientation()
{
	this->position = Vec3(0.0f, 0.0f, 0.0f);
	this->look = Vec3(0.0f, 0.0f, 1.0f);
	this->right = Vec3(1.0f, 0.0f, 0.0f);
	this->up = Vec3(0.0f, 1.0f, 0.0f);

	this->rotX = 0.0f;
	this->rotY = 0.0f;
}

Orientation::Orientation(const Orientation &other)
{
	this->position = other.position;
	this->look = other.look;
	this->right = other.right;
	this->up = other.up;
	this->rotX = other.rotX;
	this->rotY = other.rotY;
}

Orientation::~Orientation()
{
}

void Orientation::update(float dt)
{
	//D3DXVECTOR3 R, U, L, P;
	//D3DXVECTOR3 r = this->right;
	//D3DXVECTOR3 u = this->up;
	//D3DXVECTOR3 l = this->look;
	//D3DXVECTOR3 p = this->pos;



	//D3DXVec3Normalize(&L, &l);
	//D3DXVec3Normalize(&U, D3DXVec3Cross(&U, &l, &r));

	//D3DXVec3Cross(&R, &U, &l);

	//float x = -D3DXVec3Dot(&p, &R);
	//float y = -D3DXVec3Dot(&p, &U);
	//float z = -D3DXVec3Dot(&p, &L);

	//this->right = R;
	//this->up = U;
	//this->look = L;
	
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

void Orientation::strafe(float length, Vec3 &pos)
{
	this->position += this->right * length;
	pos = this->position;
}

void Orientation::walk(float length, Vec3 &pos)
{
	this->position += this->look * length;
	pos = this->position;
}

void Orientation::rotateY(float angle)
{
	//D3DXMATRIX R;
	//D3DXMatrixRotationY(&R, angle);
	//D3DXVECTOR3 u = this->up;
	//D3DXVECTOR3 l = this->look;
	//D3DXVECTOR3 r = this->right;


	//D3DXVec3TransformNormal(&this->right, &r, &R);
	//D3DXVec3TransformNormal(&this->up, &u, &R);
	//D3DXVec3TransformNormal(&this->look, &l, &R);
	this->rotY += angle;

	DirectX::XMMATRIX mRotY;
	mRotY = DirectX::XMMatrixRotationY(angle);
	DirectX::XMVECTOR u = DirectX::XMLoadFloat3(&(DirectX::XMFLOAT3(this->up.X, this->up.Y, this->up.Z)));
	DirectX::XMVECTOR l = DirectX::XMLoadFloat3(&(DirectX::XMFLOAT3(this->look.X, this->look.Y, this->look.Z)));
	DirectX::XMVECTOR r = DirectX::XMLoadFloat3(&(DirectX::XMFLOAT3(this->right.X, this->right.Y, this->right.Z)));

	r = DirectX::XMVector3TransformNormal(r, mRotY);
	DirectX::XMFLOAT3 rFloat3;
	DirectX::XMStoreFloat3(&rFloat3, r);
	this->right = Vec3(rFloat3.x, rFloat3.y, rFloat3.z);

	u = DirectX::XMVector3TransformNormal(u, mRotY);
	DirectX::XMFLOAT3 uFloat3;
	DirectX::XMStoreFloat3(&uFloat3, u);
	this->up = Vec3(uFloat3.x, uFloat3.y, uFloat3.z);

	l = DirectX::XMVector3TransformNormal(l, mRotY);
	DirectX::XMFLOAT3 lFloat3;
	DirectX::XMStoreFloat3(&lFloat3, l);
	this->look = Vec3(lFloat3.x, lFloat3.y, lFloat3.z);

}

void Orientation::rotateX(float angle)
{
	//D3DXMATRIX R;
	//D3DXMatrixRotationAxis(&R, &this->right, angle);
	//D3DXVECTOR3 u = this->up;
	//D3DXVECTOR3 l = this->look;

	//D3DXVec3TransformNormal(&this->up, &u, &R);
	//D3DXVec3TransformNormal(&this->look, &l, &R);

	this->rotX += angle;

	DirectX::XMMATRIX rotX;
	rotX = DirectX::XMMatrixRotationX(angle);
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

void Orientation::rotateAxis(Vec3 axis, float angle)
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


Vec3 Orientation::getPos() const
{
	return this->position;
}

Vec3 Orientation::getLook() const
{
	return this->look;
}

Vec3 Orientation::getRight() const
{
	return this->right;
}

Vec3 Orientation::getUp() const
{
	return this->up;
}

void Orientation::setPos(Vec3 pos)
{
	this->position = pos;
}

void Orientation::resetRotationXYZ(int Axis)
{
	switch (Axis)
	{
	case X:
		this->rotateX(-(this->rotX));
		break;
	case Y:
		this->rotateY(-(this->rotY));
		break;
	case Z:
		break;
	}
}

float Orientation::getRotX() const
{
	return this->rotX;
}

float Orientation::getRotY() const
{
	return this->rotY;
}

float Orientation::getRotZ() const
{
	return this->rotZ;
}