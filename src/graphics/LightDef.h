#ifndef LIGHTDEF_H
#define LIGHTDEF_H

#include "shared/platform.h"
#include "shared/Vec3.h"

class DLL_API DirectionalLight
{
public:
	virtual ~DirectionalLight() {};

	virtual void SetAmbient(Vec3 ambiant) = 0;
	virtual void SetDiffuse(Vec3 diffuse) = 0;
	virtual void SetSpecular(Vec3 specular) = 0;
	virtual void SetDirection(Vec3 direction) = 0;

	virtual Vec3 GetAmbient() = 0;
	virtual Vec3 GetDiffuse() = 0;
	virtual Vec3 GetSpecular() = 0;
	virtual Vec3 GetDirection() = 0;
};

class PointLight
{
public:
	virtual ~PointLight() {};

	virtual void SetAmbient(Vec3 ambiant) = 0;
	virtual void SetDiffuse(Vec3 diffuse) = 0;
	virtual void SetSpecular(Vec3 specular) = 0;
	virtual void SetAttenuation(Vec3 attenuation) = 0;
	virtual void SetPosition(Vec3 position) = 0;
	virtual void SetRange(float range) = 0;

	virtual Vec3 GetAmbient() = 0;
	virtual Vec3 GetDiffuse() = 0;
	virtual Vec3 GetSpecular() = 0;
	virtual Vec3 GetAttenuation() = 0;
	virtual Vec3 GetPosition() = 0;
	virtual float GetRange() = 0;
};

class SpotLight
{
public:
	virtual ~SpotLight() {};

	virtual void SetAmbient(Vec3 ambiant) = 0;
	virtual void SetDiffuse(Vec3 diffuse) = 0;
	virtual void SetSpecular(Vec3 specular) = 0;
	virtual void SetAttenuation(Vec3 attenuation) = 0;
	virtual void SetDirection(Vec3 direction) = 0;
	virtual void SetPosition(Vec3 position) = 0;
	virtual void SetRange(float range) = 0;
	virtual void SetSpot(float spot) = 0;

	virtual Vec3 GetAmbient() = 0;
	virtual Vec3 GetDiffuse() = 0;
	virtual Vec3 GetSpecular() = 0;
	virtual Vec3 GetAttenuation() = 0;
	virtual Vec3 GetDirection() = 0;
	virtual Vec3 GetPosition() = 0;
	virtual float GetRange() = 0;
	virtual float GetSpot() = 0;
};

#endif