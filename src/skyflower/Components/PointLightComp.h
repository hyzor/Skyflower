#ifndef POINTLIGHTCOMP_H
#define POINTLIGHTCOMP_H

#include "Cistron.h"
#include <string>
#include <iostream>
#include <sstream>
#include "shared/Vec3.h"
#include "Entity.h"

using namespace std;
using namespace Cistron;

class PointLightComp : public Component {

public:

	PointLightComp(Vec3 pos, Vec3 color, float intensity) : Component("PointLight")
	{
		this->pos = pos;
		this->color = color;
		this->intensity = intensity;

		this->lightOn = true;
	};
	virtual ~PointLightComp() 
	{
		delete light;
	};

	void addedToEntity();

	void update(float dt);

	void lit(float time);
	void unlit(float time);
	bool isLit();
	bool isUnlit();

	PointLight* light;

private:

	Vec3 pPos;

	Vec3 pos;
	Vec3 color;
	float intensity;
	float range;

	bool lightOn;
	Vec3 speed;
};

#endif