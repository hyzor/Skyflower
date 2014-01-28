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
	MoveTargetStateMovingToTarget,
	MoveTargetStateMovingToSpawn,
};

class MoveTargetComponent : public Component
{
public:
	MoveTargetComponent(Vec3 spawnPosition, Vec3 targetPosition, float duration, float easingPower, bool continuous) : Component("MoveTarget")
	{
		m_spawnPosition = spawnPosition;
		m_targetPosition = targetPosition;
		m_duration = duration;
		m_easingPower = easingPower;
		m_continuous = continuous;

		m_travelDistance = (spawnPosition - targetPosition).Length();

		m_state = MoveTargetStateIdle;
		m_progress = 1.0f;
	}

	virtual ~MoveTargetComponent()
	{
	}

	void addedToEntity()
	{
		if (m_continuous)
			moveToTarget();
	}

	void removeFromEntity()
	{
	}

	void update(float deltaTime)
	{
		if (m_state == MoveTargetStateIdle)
			return;

		m_progress += deltaTime / m_duration;
		m_progress = std::max(0.0f, std::min(1.0f, m_progress));

		float positionalProgress;

		if (m_progress < 0.5f)
		{
			// Ease in
			positionalProgress = powf(m_progress * 2.0f, m_easingPower) * 0.5f;
		}
		else
		{
			// Ease out
			positionalProgress = (1.0f - powf(1.0f - (m_progress - 0.5f) * 2.0f, m_easingPower)) * 0.5f + 0.5f;
		}

		Vec3 targetPosition = (m_state == MoveTargetStateMovingToTarget? m_targetPosition : m_spawnPosition);
		Vec3 direction = (targetPosition - getEntityPos()).Normalize();
		Vec3 position = targetPosition - direction * m_travelDistance * (1.0f - positionalProgress);

		updateEntityPos(position);

		if (m_progress >= 1.0f)
		{
			if (!m_continuous)
			{
				m_state = MoveTargetStateIdle;
			}
			else
			{
				switch(m_state)
				{
				case MoveTargetStateMovingToSpawn:
					moveToTarget();
					break;
				case MoveTargetStateMovingToTarget:
					moveToSpawn();
					break;
				default:
					assert(0);
					break;
				}
			}
		}
	}

	void moveToSpawn()
	{
		m_progress = 1.0f - (getEntityPos() - m_spawnPosition).Length() / m_travelDistance;
		m_state = MoveTargetStateMovingToSpawn;
	}

	void moveToTarget()
	{
		m_progress = 1.0f - (getEntityPos() - m_targetPosition).Length() / m_travelDistance;
		m_state = MoveTargetStateMovingToTarget;
	}

	void setContinuous(bool continuous)
	{
		m_continuous = continuous;
	}

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
