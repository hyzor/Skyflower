#ifndef SHADOWCOMP_H
#define SHADOWCOMP_H

#include "Cistron.h"
#include <iostream>
#include "shared/Vec3.h"
#include "Entity.h"
using namespace std;
using namespace Cistron;

class ShadowComp : public Component
{
public:
	ShadowComp();
	virtual ~ShadowComp();

	void addedToEntity();
	void removeFromEntity();

	void update(float deltaTime);

private:
	ModelInstance* shadowmodel;
};

#endif
