#ifndef PUSH_H
#define PUSH_H

#include <cstdio>
#include <string>

#include "shared/Vec3.h"
#include "Cistron.h"
#include "Entity.h"

using namespace std;
using namespace Cistron;

class Push : public Component {

public:
	Push() : Component("Push")
	{
	}

	virtual ~Push()
	{
	}

	void addedToEntity()
	{
	}

	void removeFromEntity()
	{
	}

	void update(float deltaTime)
	{

	}

private:

};

#endif