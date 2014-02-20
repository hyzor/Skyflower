#include "Components/PointLightComp.h"
#include "EntityManager.h"

// Must be included last!
#include "shared/debug.h"

void PointLightComp::addedToEntity()
{
	light = getEntityManager()->modules->graphics->AddPointLight(color, getOwner()->returnPos() + pos, intensity);
	color = light->GetDiffuse();
}

void PointLightComp::update(float dt)
{
	light->SetPosition(getOwner()->returnPos() + pos);

	if (lightOn)
	{
		Vec3 diff = light->GetDiffuse();
		diff += speed*dt;
		if (diff.X > color.X)
			diff.X = color.X;
		if (diff.Y > color.Y)
			diff.Y = color.Y;
		if (diff.Z > color.Z)
			diff.Z = color.Z;

		light->SetDiffuse(diff);
		light->SetAmbient(diff);
		light->SetSpecular(diff);
	}
	else
	{
		Vec3 diff = light->GetDiffuse();
		diff -= speed*dt;
		if (diff.X < 0)
			diff.X = 0;
		if (diff.Y < 0)
			diff.Y = 0;
		if (diff.Z < 0)
			diff.Z = 0;

		light->SetDiffuse(diff);
		light->SetAmbient(diff);
		light->SetSpecular(diff);
	}
}

void PointLightComp::lit(float time)
{
	lightOn = true;
	speed = color / time;
}

void PointLightComp::unlit(float time)
{
	lightOn = false;
	speed = color / time;
}

bool PointLightComp::isLit()
{
	return light->GetDiffuse() == color;
}

bool PointLightComp::isUnlit()
{
	return (light->GetDiffuse() == Vec3());
}
