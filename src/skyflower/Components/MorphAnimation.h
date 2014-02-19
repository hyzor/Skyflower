#ifndef MORPHANIMATION_H
#define MORPHANIMATION_H

#include "shared/Vec3.h"
#include "graphics/instance.h"
#include "Cistron.h"
#include "Entity.h"
#include "Component.h"
#include <string>

using namespace std;
using namespace Cistron;

class MorphAnimation : public Component
{
public:
	MorphAnimation(string path, string model) : Component("MorphAnimation")
	{
		this->path = path;
		this->model = model;
		m_model = NULL;
		isMorphing = false;
		speed = 0.0f;
	};
	virtual ~MorphAnimation(){}

	void addedToEntity();
	void removeFromEntity();
	void update(float deltaTime);
	void startMorphing(Vec3 targetWeight, float speed);
	void setWeights(Vec3 weight);
	Vec3 getWeights() const;

private:
	string path;
	string model;
	MorphModelInstance *m_model;
	Vec3 morphSpeed;
	Vec3 targetWeight;
	bool isMorphing;
	float speed;
};

#endif
