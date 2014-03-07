#include "PortalEffect.h"
#include "Entity.h"

// Must be included last!
#include "shared/debug.h"

PortalEffectComponent::PortalEffectComponent() : Component("PortalEffect")
{
}

PortalEffectComponent::~PortalEffectComponent()
{
}

void PortalEffectComponent::addedToEntity()
{
	Vec3 position = getOwner()->returnPos();

	for (int i = 0; i < PORTAL_EFFECT_PARTICLE_SYSTEMS; i++) {
		m_particleSystems[i] = getOwner()->getModules()->graphics->CreateParticleSystem();
		m_particleSystems[i]->SetActive(true);
		m_particleSystems[i]->SetParticleType(ParticleType::PT_PORTAL);
		m_particleSystems[i]->SetEmitFrequency(1.0f / 25.0f);
		m_particleSystems[i]->SetParticleAgeLimit(1.0f);
		m_particleSystems[i]->SetParticleFadeTime(1.0f);
		m_particleSystems[i]->SetScale(XMFLOAT2(3.0f, 3.0f));
		m_particleSystems[i]->SetConstantAccel(XMFLOAT3(0.0f, 0.0f, 0.0f));
		m_particleSystems[i]->SetRandomVelocityActive(true);
		m_particleSystems[i]->SetRandomVelocity(XMFLOAT3(1.0f, 1.0f, 1.0f));
		m_particleSystems[i]->SetEmitPos(XMFLOAT3(position.X, position.Y, position.Z));

		m_animationTime[i] = PORTAL_EFFECT_DURATION * ((float)i / PORTAL_EFFECT_PARTICLE_SYSTEMS);
	}

	float yaw = -getOwner()->returnRot().Y + XM_PIDIV2;

	m_forward = Vec3(cosf(yaw), 0.0f, sinf(yaw)).Normalize();
	m_up = Vec3(0.0f, 1.0f, 0.0f);
	m_right = m_forward.Cross(m_up);
}

void PortalEffectComponent::removeFromEntity()
{
	for (int i = 0; i < PORTAL_EFFECT_PARTICLE_SYSTEMS; i++) {
		getOwner()->getModules()->graphics->DeleteParticleSystem(m_particleSystems[i]);
		m_particleSystems[i] = NULL;
	}
}

void PortalEffectComponent::update(float deltaTime)
{
	static const float maxDistance = 6.0f;

	Vec3 position = getOwner()->returnPos();

	for (int i = 0; i < PORTAL_EFFECT_PARTICLE_SYSTEMS; i++) {
		m_animationTime[i] += deltaTime;

		if (m_animationTime[i] > PORTAL_EFFECT_DURATION) {
			m_animationTime[i] = 0.0f;
		}

		float animationProgress = m_animationTime[i] / PORTAL_EFFECT_DURATION;

		float angle = m_animationTime[i] * XM_2PI;
		//float distance = animationProgress * maxDistance;
		float distance = 3.0f;

		float x = 0.0f;
		float y = 17.0f;
		float z = 2.0f;

		x += cosf(angle) * distance;
		y += sinf(angle) * distance;

		Vec3 particleSystemPosition = position;
		particleSystemPosition += m_right * x;
		particleSystemPosition += m_up * y;
		particleSystemPosition += m_forward * z;

		m_particleSystems[i]->SetEmitPos(XMFLOAT3(particleSystemPosition.X, particleSystemPosition.Y, particleSystemPosition.Z));
	}
}
