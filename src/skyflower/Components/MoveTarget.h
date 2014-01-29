#ifndef COMPONENT_MOVETARGET_H
#define COMPONENT_MOVETARGET_H

#include "shared/Vec3.h"

#include "Cistron.h"
#include "Component.h"

using namespace Cistron;

enum MoveTargetState
{
	MoveTargetStateIdle = 0,
	MoveTargetStateMovingToTarget,
	MoveTargetStateMovingToSpawn,
};

class MoveTargetComponent : public Component
{
public:
	MoveTargetComponent(Vec3 spawnPosition, Vec3 targetPosition, float duration, float easingPower, bool continuous);
	virtual ~MoveTargetComponent();

	void addedToEntity();
	void removeFromEntity();

	void update(float deltaTime);

	void moveToSpawn();
	void moveToTarget();
	void setContinuous(bool continuous);

private:
	float GetPositionalProgress(float progress);

private:
	Vec3 m_spawnPosition;
	Vec3 m_targetPosition;
	float m_duration;
	float m_easingPower;
	bool m_continuous;

	float m_travelDistance;

	MoveTargetState m_state;
	float m_progress;
};

#endif
