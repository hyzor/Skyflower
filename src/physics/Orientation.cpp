#include "Orientation.h"

Orientation::Orientation()
{
	this->position = Vec3(0.0f, 0.0f, 0.0f);
	this->look = Vec3(0.0f, 0.0f, 0.0f);
	this->right = Vec3(0.0f, 0.0f, 0.0f);
	this->up = Vec3(0.0f, 0.0f, 0.0f);
}

Orientation::Orientation(const Orientation &other)
{
	this->position = other.position;
	this->look = other.look;
	this->right = other.right;
	this->up = other.up;
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
	
	DirectX::XMVECTOR r = DirectX::XMLoadFloat3(&(DirectX::XMFLOAT3(this->right.X, this->right.Y, this->right.Z)));
	DirectX::XMVECTOR u = DirectX::XMLoadFloat3(&(DirectX::XMFLOAT3(this->up.X, this->up.Y, this->up.Z)));
	DirectX::XMVECTOR l = DirectX::XMLoadFloat3(&(DirectX::XMFLOAT3(this->look.X, this->look.Y, this->look.Z)));
	DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&(DirectX::XMFLOAT3(this->position.X, this->position.Y, this->position.Z)));

	l = DirectX::XMVector3Normalize(l);
	r = DirectX::XMVector3Normalize(r);
	u = DirectX::XMVector3Normalize(u);
	
}

void Orientation::strafe(float length)
{
	this->position += length * this->right;
}

void Orientation::walk(float length)
{
	this->position += length * this->look;
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
}

void Orientation::rotateX(float angle)
{
	//D3DXMATRIX R;
	//D3DXMatrixRotationAxis(&R, &this->right, angle);
	//D3DXVECTOR3 u = this->up;
	//D3DXVECTOR3 l = this->look;

	//D3DXVec3TransformNormal(&this->up, &u, &R);
	//D3DXVec3TransformNormal(&this->look, &l, &R);
}

void Orientation::rotateAxis(Vec3 axis, float angle)
{
	//D3DXMATRIX R;
	//D3DXMatrixRotationAxis(&R, &axis, angle);
	//D3DXVECTOR3 u = this->up;
	//D3DXVECTOR3 l = this->look;

	//D3DXVec3TransformNormal(&this->up, &u, &R);
	//D3DXVec3TransformNormal(&this->look, &l, &R);
}
