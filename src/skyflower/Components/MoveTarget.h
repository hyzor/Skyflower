#ifndef COMPONENT_MOVETARGET_H
#define COMPONENT_MOVETARGET_H

#include <algorithm>
#include <string>

#include "shared/Vec3.h"
#include "Cistron.h"
#include "Entity.h"

using namespace std;
using namespace Cistron;

enum MoveTargetState
{
	MoveTargetStateIdle = 0,
	MoveTargetStateMovingToEnd,
	MoveTargetStateMovingToStart,
};

class MoveTargetComponent : public Component
{
public:
	MoveTargetComponent(Vec3 startPosition, Vec3 endPosition, float duration) : Component("MoveTarget")
	{
		m_startPosition = startPosition;
		m_endPosition = endPosition;
		m_duration = duration;
		m_travelDistance = (startPosition - endPosition).Length();

		m_state = MoveTargetStateIdle;
		m_progress = 1.0f;
	}

	virtual ~MoveTargetComponent()
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
		if (m_state == MoveTargetStateIdle)
			return;

		m_progress += deltaTime / m_duration;

		float easingPower = 4.0f;
		float positionalProgress;

		if (m_progress < 0.5f)
			positionalProgress = powf(m_progress * 2.0f, easingPower) * 0.5f;
		else
			positionalProgress = (1.0f - powf(1.0f - (m_progress - 0.5f) * 2.0f, easingPower)) * 0.5f + 0.5f;

		printf("progress=%.4f, positionalProgress=%.4f\n", m_progress, positionalProgress);

		Vec3 targetPosition = (m_state == MoveTargetStateMovingToEnd? m_endPosition : m_startPosition);
		Vec3 direction = (targetPosition - getEntityPos()).Normalize();
		//Vec3 position = targetPosition - direction * m_travelDistance * positionalProgress;
		Vec3 position = targetPosition - direction * m_travelDistance * (1.0f - positionalProgress);

		updateEntityPos(position);

		if (m_progress >= 1.0f)
			m_state = MoveTargetStateIdle;
	}

	void moveToStart()
	{
		m_progress = 1.0f - (getEntityPos() - m_startPosition).Length() / m_travelDistance;
		m_state = MoveTargetStateMovingToStart;
	}

	void moveToEnd()
	{
		m_progress = 1.0f - (getEntityPos() - m_endPosition).Length() / m_travelDistance;
		m_state = MoveTargetStateMovingToEnd;
	}

private:
	Vec3 m_startPosition;
	Vec3 m_endPosition;
	float m_duration;
	float m_travelDistance;

	MoveTargetState m_state;
	float m_progress;
};

#endif
