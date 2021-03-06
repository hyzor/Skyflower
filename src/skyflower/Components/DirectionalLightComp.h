#ifndef DIRECTIONALLIGHTCOMP_H
#define DIRECTIONALLIGHTCOMP_H

#include "Cistron.h"
#include <string>
#include <iostream>
#include <sstream>
#include "shared/Vec3.h"
#include "Entity.h"

using namespace std;
using namespace Cistron;

class DirectionalLightComp : public Component {

public:

	DirectionalLightComp(Vec3 color, Vec3 dir, float intensity) : Component("DirectionalLight")
	{
		this->color = color;
		this->dir = dir;
		this->intensity = intensity;

		this->lightOn = true;
	};
	virtual ~DirectionalLightComp() 
	{
		delete light;
	};

	void addedToEntity();
	void update(float dt);
	void lit(float time);
	void unlit(float time);

	bool isLit();
	bool isUnlit();

	DirectionalLight* light;

private:

	Vec3 color;
	Vec3 dir;
	Vec3 speed;

	float intensity;

	bool lightOn;

};

#endif