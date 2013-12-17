#ifndef TRIGGER_H
#define TRIGGER_H

#include <cstdio>
#include <string>

#include "shared/Vec3.h"
#include "Cistron.h"
#include "Entity.h"

using namespace std;
using namespace Cistron;

class Trigger : public Component {

public:
	Trigger(): Component("Trigger")
	{
	}

	virtual ~Trigger()
	{

	}

	void addedToEntity()
	{
		this->startPosition = getEntityPos();
	}

	void removeFromEntity()
	{
	}

	void update(float deltaTime)
	{

	}

private:
	Vec3 startPosition;
};

#endif