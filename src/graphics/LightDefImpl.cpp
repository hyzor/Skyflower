#include "LightDefImpl.h"


DirectionalLightImpl::DirectionalLightImpl(DLight* light)
{
	this->light = light;
	ZeroMemory(this->light, sizeof(DLight));
}
DirectionalLightImpl::~DirectionalLightImpl()
{
}

void DirectionalLightImpl::SetAmbient(Vec3 ambient)
{
	light->Ambient.x = ambient.X;
	light->Ambient.y = ambient.Y;
	light->Ambient.z = ambient.Z;
	light->Ambient.w = 1.0f;
}
void DirectionalLightImpl::SetDiffuse(Vec3 diffuse)
{
	light->Diffuse.x = diffuse.X;
	light->Diffuse.y = diffuse.Y;
	light->Diffuse.z = diffuse.Z;
	light->Diffuse.w = 1.0f;
}
void DirectionalLightImpl::SetSpecular(Vec3 specular)
{
	light->Specular.x = specular.X;
	light->Specular.y = specular.Y;
	light->Specular.z = specular.Z;
	light->Specular.w = 1.0f;
}
void DirectionalLightImpl::SetDirection(Vec3 direction)
{
	light->Direction.x = direction.X;
	light->Direction.y = direction.Y;
	light->Direction.z = direction.Z;
}

Vec3 DirectionalLightImpl::GetAmbient()
{
	return Vec3(light->Ambient.x, light->Ambient.y, light->Ambient.z);
}
Vec3 DirectionalLightImpl::GetDiffuse()
{
	return Vec3(light->Diffuse.x, light->Diffuse.y, light->Diffuse.z);
}
Vec3 DirectionalLightImpl::GetSpecular()
{
	return Vec3(light->Specular.x, light->Specular.y, light->Specular.z);
}
Vec3 DirectionalLightImpl::GetDirection()
{
	return Vec3(light->Direction.x, light->Direction.y, light->Direction.z);
}




PointLightImpl::PointLightImpl(PLight* light)
{
	this->light = light;
	ZeroMemory(this->light, sizeof(PLight));
}
PointLightImpl::~PointLightImpl()
{
}

void PointLightImpl::SetAmbient(Vec3 ambient)
{
	light->Ambient.x = ambient.X;
	light->Ambient.y = ambient.Y;
	light->Ambient.z = ambient.Z;
	light->Ambient.w = 1.0f;
}
void PointLightImpl::SetDiffuse(Vec3 diffuse)
{
	light->Diffuse.x = diffuse.X;
	light->Diffuse.y = diffuse.Y;
	light->Diffuse.z = diffuse.Z;
	light->Diffuse.w = 1.0f;
}
void PointLightImpl::SetSpecular(Vec3 specular)
{
	light->Specular.x = specular.X;
	light->Specular.y = specular.Y;
	light->Specular.z = specular.Z;
	light->Specular.w = 1.0f;
}
void PointLightImpl::SetAttenuation(Vec3 attenuation)
{
	light->Attenuation.x = attenuation.X;
	light->Attenuation.y = attenuation.Y;
	light->Attenuation.z = attenuation.Z;
}
void PointLightImpl::SetPosition(Vec3 position)
{
	light->Position.x = position.X;
	light->Position.y = position.Y;
	light->Position.z = position.Z;
}
void PointLightImpl::SetRange(float range)
{
	light->Range = range;
}

Vec3 PointLightImpl::GetAmbient()
{
	return Vec3(light->Ambient.x, light->Ambient.y, light->Ambient.z);
}
Vec3 PointLightImpl::GetDiffuse()
{
	return Vec3(light->Diffuse.x, light->Diffuse.y, light->Diffuse.z);
}
Vec3 PointLightImpl::GetSpecular()
{
	return Vec3(light->Specular.x, light->Specular.y, light->Specular.z);
}
Vec3 PointLightImpl::GetAttenuation()
{
	return Vec3(light->Attenuation.x, light->Attenuation.y, light->Attenuation.z);
}
Vec3 PointLightImpl::GetPosition()
{
	return Vec3(light->Position.x, light->Position.y, light->Position.z);
}
float PointLightImpl::GetRange()
{
	return light->Range;
}




SpotLightImpl::SpotLightImpl(SLight* light)
{
	this->light = light;
	ZeroMemory(this->light, sizeof(SLight));
}
SpotLightImpl::~SpotLightImpl()
{
}
void SpotLightImpl::SetAmbient(Vec3 ambient)
{
	light->Ambient.x = ambient.X;
	light->Ambient.y = ambient.Y;
	light->Ambient.z = ambient.Z;
	light->Ambient.w = 1.0f;
}
void SpotLightImpl::SetDiffuse(Vec3 diffuse)
{
	light->Diffuse.x = diffuse.X;
	light->Diffuse.y = diffuse.Y;
	light->Diffuse.z = diffuse.Z;
	light->Diffuse.w = 1.0f;
}
void SpotLightImpl::SetSpecular(Vec3 specular)
{
	light->Specular.x = specular.X;
	light->Specular.y = specular.Y;
	light->Specular.z = specular.Z;
	light->Specular.w = 1.0f;
}
void SpotLightImpl::SetAttenuation(Vec3 attenuation)
{
	light->Attenuation.x = attenuation.X;
	light->Attenuation.y = attenuation.Y;
	light->Attenuation.z = attenuation.Z;
}
void SpotLightImpl::SetDirection(Vec3 direction)
{
	light->Direction.x = direction.X;
	light->Direction.y = direction.Y;
	light->Direction.z = direction.Z;
}
void SpotLightImpl::SetPosition(Vec3 position)
{
	light->Position.x = position.X;
	light->Position.y = position.Y;
	light->Position.z = position.Z;
}
void SpotLightImpl::SetRange(float range)
{
	light->Range = range;
}
void SpotLightImpl::SetSpot(float spot)
{
	light->Spot = spot;
}

Vec3 SpotLightImpl::GetAmbient()
{
	return Vec3(light->Ambient.x, light->Ambient.y, light->Ambient.z);
}
Vec3 SpotLightImpl::GetDiffuse()
{
	return Vec3(light->Diffuse.x, light->Diffuse.y, light->Diffuse.z);
}
Vec3 SpotLightImpl::GetSpecular()
{
	return Vec3(light->Specular.x, light->Specular.y, light->Specular.z);
}
Vec3 SpotLightImpl::GetAttenuation()
{
	return Vec3(light->Attenuation.x, light->Attenuation.y, light->Attenuation.z);
}
Vec3 SpotLightImpl::GetDirection()
{
	return Vec3(light->Direction.x, light->Direction.y, light->Direction.z);
}
Vec3 SpotLightImpl::GetPosition()
{
	return Vec3(light->Position.x, light->Position.y, light->Position.z);
}
float SpotLightImpl::GetRange()
{
	return light->Range;
}
float SpotLightImpl::GetSpot()
{
	return light->Spot;
}