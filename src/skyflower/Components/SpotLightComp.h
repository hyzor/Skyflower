#ifndef SPOTLIGHTCOMP_H
#define SPOTLIGHTCOMP_H

#include "Cistron.h"
#include <string>
#include <iostream>
#include <sstream>
#include "shared/Vec3.h"
#include "Entity.h"

using namespace std;
using namespace Cistron;

class SpotLightComp : public Component {

public:

	SpotLightComp(Vec3 pos, Vec3 color, float angle, Vec3 dir) : Component("SpotLight")
	{
		this->pos = pos;
		this->color = color;
		this->dir = dir;
		this->angle = angle;

		this->lightOn = true;
	};
	virtual ~SpotLightComp() {};

	void addedToEntity();

	void sendAMessage(string message)
	{
		sendMessage(message);
	}

	void update(float dt);

	void lit(float time);
	void unlit(float time);
	bool isLit();
	bool isUnlit();

	SpotLight* light;

private:

	Vec3 pos;
	Vec3 color;
	Vec3 dir;
	float angle;

	bool lightOn;
	Vec3 speed;
};

#endif