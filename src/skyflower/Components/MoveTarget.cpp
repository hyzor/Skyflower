#include <algorithm>

#include "shared/Vec3.h"

#include "MoveTarget.h"

MoveTargetComponent::MoveTargetComponent(Vec3 spawnPosition, Vec3 targetPosition, float duration, float easingPower, bool continuous) : Component("MoveTarget")
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

MoveTargetComponent::~MoveTargetComponent()
{
}

void MoveTargetComponent::addedToEntity()
{
	if (m_continuous)
		moveToTarget();
}

void MoveTargetComponent::removeFromEntity()
{
}

void MoveTargetComponent::update(float deltaTime)
{
	if (m_state == MoveTargetStateIdle)
		return;

	m_progress += deltaTime / m_duration;
	m_progress = std::max(0.0f, std::min(1.0f, m_progress));

	float positionalProgress = GetPositionalProgress(m_progress);
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

void MoveTargetComponent::moveToSpawn()
{
	// FIXME: This is positional progress!
	m_progress = 1.0f - (getEntityPos() - m_spawnPosition).Length() / m_travelDistance;
	m_state = MoveTargetStateMovingToSpawn;
}

void MoveTargetComponent::moveToTarget()
{
	m_progress = 1.0f - (getEntityPos() - m_targetPosition).Length() / m_travelDistance;
	m_state = MoveTargetStateMovingToTarget;
}

void MoveTargetComponent::setContinuous(bool continuous)
{
	m_continuous = continuous;
}

float MoveTargetComponent::GetPositionalProgress(float progress)
{
	if (progress < 0.5f)
	{
		// Ease in
		return powf(progress * 2.0f, m_easingPower) * 0.5f;
	}
	else
	{
		// Ease out
		return (1.0f - powf(1.0f - (progress - 0.5f) * 2.0f, m_easingPower)) * 0.5f + 0.5f;
	}
}
