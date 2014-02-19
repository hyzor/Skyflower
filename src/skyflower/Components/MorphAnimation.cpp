#include "MorphAnimation.h"

// Must be included last!
#include "shared/debug.h"

void MorphAnimation::addedToEntity()
{
	Vec3 pos = getOwner()->returnPos();
	Vec3 rot = getOwner()->returnRot();
	Vec3 scale = getOwner()->returnScale();

	m_model = getOwner()->getModules()->graphics->CreateMorphAnimatedInstance(path, model, Vec3());
	m_model->Set(pos, rot, scale, Vec3(0.0f, 0.0f, 0.0f));
	m_model->SetVisibility(getOwner()->returnVisible());
}

void MorphAnimation::removeFromEntity()
{
	if (m_model)
		getOwner()->getModules()->graphics->DeleteInstance(m_model);
}

void MorphAnimation::update(float dt)
{
	/*if (getOwner()->returnVisible() != m_model->IsVisible())
	{
		m_model->SetVisibility(getOwner()->returnVisible());
	}*/
	if (isMorphing)
	{
		Vec3 weight = m_model->GetWeights();
		weight += morphSpeed * dt;

		if (abs((weight - targetWeight).Length()) > 0.1)
		{
			m_model->SetWeights(weight);
		}
		else
		{
			m_model->SetWeights(targetWeight);
			this->isMorphing = false;
		}
	}
}

void MorphAnimation::startMorphing(Vec3 targetWeight, float speed)
{
	this->targetWeight = targetWeight;
	this->morphSpeed = (targetWeight - m_model->GetWeights()) * speed;
	this->isMorphing = true;
	this->speed = speed;
}

void MorphAnimation::setWeights(Vec3 weights)
{
	m_model->SetWeights(weights);
}

Vec3 MorphAnimation::getWeights() const
{
	return m_model->GetWeights();
}
