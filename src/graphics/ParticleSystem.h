#ifndef PARTICLESYSTEM_H_
#define PARTICLESYSTEM_H_

#include <DirectXMath.h>

using namespace DirectX;

// Must be mirrored with "ParticleSystemShared.hlsli"
static const enum ParticleType
{
	PT_EMITTER,
	PT_FLARE0,
	PT_FLARE1,
	PT_PARTICLE,
	PT_BIRD,
	PT_DUST,
	PT_PORTAL,
	PT_AIM,
	PT_REDSTAR,
	PT_BLUESTAR,
	PT_WHITESTAR,
	PT_POLLEN,
	NROFTYPES
};

class ParticleSystem
{
public:
	virtual ~ParticleSystem() {}

	virtual void SetActive(bool active) = 0;
	virtual bool IsActive() const = 0;

	virtual void SetEmitPos(const XMFLOAT3& emitPosW) = 0;
	virtual void SetEmitDir(const XMFLOAT3& emitDirW) = 0;

	virtual void SetConstantAccel(XMFLOAT3 accelW) = 0;
	virtual void SetParticleFadeTime(float fadeTime) = 0;

	virtual void SetBlendingMethod(unsigned int blendingMethod) = 0;

	virtual void SetEmitFrequency(float emitFrequency) = 0;
	virtual void SetParticleAgeLimit(float particleAgeLimit) = 0;

	virtual void SetParticleType(ParticleType particleType) = 0;
	virtual void SetScale(XMFLOAT2 scale) = 0;

	virtual void SetRandomVelocityActive(bool active) = 0;
	virtual void SetRandomVelocity(XMFLOAT3 randomVelocity) = 0;

	virtual void SetColor(XMFLOAT3 color) = 0;

	virtual float GetAgeLimit() const = 0;
	virtual float GetEmitFrequency() const = 0;

	virtual void Reset() = 0;
};

#endif
