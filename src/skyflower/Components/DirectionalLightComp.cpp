#include "Components/DirectionalLightComp.h"
#include "EntityManager.h"

// Must be included last!
#include "shared/debug.h"

void DirectionalLightComp::addedToEntity()
{
	light = getEntityManager()->modules->graphics->addDirLight(color, dir, intensity);
	color = light->GetDiffuse();
}

void DirectionalLightComp::update(float dt)
{
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
		light->SetSpecular(diff);
	}
}


void DirectionalLightComp::lit(float time)
{
	lightOn = true;
	speed = color / time;
}

void DirectionalLightComp::unlit(float time)
{
	lightOn = false;
	speed = color / time;
}

bool DirectionalLightComp::isLit()
{
	return (light->GetDiffuse() == color);
}

bool DirectionalLightComp::isUnlit()
{
	return (light->GetDiffuse() == Vec3());
}
