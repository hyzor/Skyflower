//-------------------------------------------------------------------------------------------
// File: LightDef.h
//
// This class is only used to define lights
//-------------------------------------------------------------------------------------------

#ifndef LIGHTDEFIMPL_H
#define LIGHTDEFIMPL_H

#include <windows.h>
#include <DirectXMath.h>
#include "LightDef.h"

using namespace DirectX;

struct DLight
{
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;

	// Forms into a 4D vector
	XMFLOAT3 Direction;
	float Padding;
};

struct PLight
{
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;

	// Forms into a 4D vector
	XMFLOAT3 Position;
	float Range;

	// Forms into a 4D vector
	XMFLOAT3 Attenuation;
	float Padding;
};

struct SLight
{
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;

	// Forms into a 4D vector
	XMFLOAT3 Position;
	float Range;

	// Forms into a 4D vector
	XMFLOAT3 Direction;
	float Spot;

	// Forms into a 4D vector
	XMFLOAT3 Attenuation;
	float Padding;
};


class DirectionalLightImpl : public DirectionalLight
{
public:
	DirectionalLightImpl(DLight* light);
	~DirectionalLightImpl();

	void SetAmbient(Vec3 ambient);
	void SetDiffuse(Vec3 diffuse);
	void SetSpecular(Vec3 specular);
	void SetDirection(Vec3 direction);

	Vec3 GetAmbient();
	Vec3 GetDiffuse();
	Vec3 GetSpecular();
	Vec3 GetDirection();


	DLight* light;
};

class PointLightImpl : public PointLight
{
public:
	PointLightImpl(PLight* light);
	~PointLightImpl();

	void SetAmbient(Vec3 ambiant);
	void SetDiffuse(Vec3 diffuse);
	void SetSpecular(Vec3 specular);
	void SetAttenuation(Vec3 attenuation);
	void SetPosition(Vec3 position);
	void SetRange(float range);

	Vec3 GetAmbient();
	Vec3 GetDiffuse();
	Vec3 GetSpecular();
	Vec3 GetAttenuation();
	Vec3 GetPosition();
	float GetRange();


	PLight* light;
};

class SpotLightImpl : public SpotLight
{
public:
	SpotLightImpl(SLight* light);
	~SpotLightImpl();

	void SetAmbient(Vec3 ambiant);
	void SetDiffuse(Vec3 diffuse);
	void SetSpecular(Vec3 specular);
	void SetAttenuation(Vec3 attenuation);
	void SetDirection(Vec3 direction);
	void SetPosition(Vec3 position);
	void SetRange(float range);
	void SetSpot(float spot);

	Vec3 GetAmbient();
	Vec3 GetDiffuse();
	Vec3 GetSpecular();
	Vec3 GetAttenuation();
	Vec3 GetDirection();
	Vec3 GetPosition();
	float GetRange();
	float GetSpot();

	SLight* light;
};




struct Material
{
	Material() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular; // w = SpecPower
	XMFLOAT4 Reflect;
};

#endif