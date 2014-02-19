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
	virtual ~DirectionalLightComp() {};

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

	DirectionalLight* light;

private:

	Vec3 color;
	Vec3 dir;
	float intensity;

	bool lightOn;
	Vec3 speed;
};

#endif