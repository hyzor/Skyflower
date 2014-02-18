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

class MorphAnimtation : public Component
{
public:
	MorphAnimtation(string path, string model) : Component("MorphAnimation")
	{
		isMorphing = false;
		speed = 0.0f;
		this->path = path;
		this->model = model;
	};
	virtual ~MorphAnimtation();

	void addedToEntity();
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
