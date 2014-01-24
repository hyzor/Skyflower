#ifndef COMPONENT_MOVETARGET_H
#define COMPONENT_MOVETARGET_H

#include <algorithm>
#include <string>

#include "shared/Vec3.h"
#include "Cistron.h"
#include "Entity.h"

using namespace std;
using namespace Cistron;

static void cubic_bezier_curve(float t, const float p0[2], const float p1[2], const float p2[2], const float p3[2], float output[2])
{
	assert(t >= 0.0f);
	assert(t <= 1.0f);
	
	float a = 1.0f - t;

	output[0] = 0.0f;
	//output[0] = a * a * a * p0[0] + 3.0f * a * a * t * p1[0] + 3.0f * a * t * t * p2[0] + t * t * t * p3[0];
	output[1] = a * a * a * p0[1] + 3.0f * a * a * t * p1[1] + 3.0f * a * t * t * p2[1] + t * t * t * p3[1];
}

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

		if (m_progress >= 1.0f)
		{
			m_state = MoveTargetStateIdle;
		}
		else
		{
			// Bezier curve control points.
			// Use http://blogs.sitepointstatic.com/examples/tech/canvas-curves/bezier-curve.html to visualize the curve.
			static float p0[2] = {0.0f, 0.0f};
			static float p1[2] = {1.0f, 0.0f};
			static float p2[2] = {0.0f, 1.0f};
			static float p3[2] = {1.0f, 1.0f};

			m_progress += deltaTime / m_duration;

			float bezier_output[2];
			float t = 1.0f - std::min(1.0f, std::max(0.0f, m_progress));

			cubic_bezier_curve(t, p0, p1, p2, p3, bezier_output);

			//printf("progress=%.4f, bezier_output[1]=%.4f\n", m_progress, bezier_output[1]);

			Vec3 targetPosition = (m_state == MoveTargetStateMovingToEnd? m_endPosition : m_startPosition);
			Vec3 direction = (targetPosition - getEntityPos()).Normalize();
			Vec3 position = targetPosition - direction * m_travelDistance * bezier_output[1];

			updateEntityPos(position);
		}
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
