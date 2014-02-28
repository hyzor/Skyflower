#ifndef PARTICLESYSTEMIMPL_H_
#define PARTICLESYSTEMIMPL_H_

#include "ParticleSystem.h"
#include "ShaderHandler.h"
#include "Camera.h"
#include "Vertex.h"

using namespace DirectX;

// Geometry shader stream-out declaration entries
class GeoStreamOutDesc
{
public:
	static D3D11_SO_DECLARATION_ENTRY ParticleSoDesc[5];
	static UINT ParticleStride;
};

class ParticleSystemImpl : public ParticleSystem
{
public:
	ParticleSystemImpl();
	virtual ~ParticleSystemImpl();

	void SetActive(bool active);
	bool IsActive() const;

	float GetAge() const;

	void SetEyePos(const XMFLOAT3& eyePosW);
	void SetEmitPos(const XMFLOAT3& emitPosW);
	void SetEmitDir(const XMFLOAT3& emitDirW);
	void SetConstantAccel(XMFLOAT3 accelW);
	void SetParticleFadeTime(float fadeTime);
	void SetEmitFrequency(float emitFrequency);
	void SetParticleAgeLimit(float particleAgeLimit);
	void SetBlendingMethod(unsigned int blendingMethod);
	void SetScale(XMFLOAT2 scale);
	void SetParticleType(ParticleType particleType);
	void SetRandomVelocityActive(bool active);
	void SetRandomVelocity(XMFLOAT3 randomVelocity);
	void SetColor(XMFLOAT3 color);

	float GetAgeLimit() const;

	void Init(ID3D11Device* device,
		ParticleSystemShader* shader,
		ID3D11ShaderResourceView* texArraySRV,
		ID3D11ShaderResourceView* randomTexSRV,
		UINT maxParticles);

	void Reset();
	void Update(float dt, float gameTime);
	void Draw(ID3D11DeviceContext* dc, const Camera& cam);

private:
	void BuildVB(ID3D11Device* device);
	
private:
	bool mActive;

	UINT mMaxParticles;
	bool mFirstRun;

	float mTimeStep;
	float mGameTime;
	float mAge;

	XMFLOAT3 mEyePosW;
	XMFLOAT3 mEmitPosW;
	XMFLOAT3 mEmitDirW;

	XMFLOAT3 mConstantAccelW;

	float mFadeTime;

	UINT mBlendingMethod;

	ParticleSystemShader* mShader;

	ID3D11Buffer* mInitVB;
	ID3D11Buffer* mDrawVB;
	ID3D11Buffer* mStreamOutVB;

	ID3D11ShaderResourceView* mTexArraySRV;
	ID3D11ShaderResourceView* mRandomTexSRV;

	UINT mNrOfTextures;

	float mParticleAgeLimit;
	float mEmitFrequency;

	UINT mParticleType;

	XMFLOAT2 mScale;

	bool mRandomizeVelocity;
	XMFLOAT3 mRandomVelocity;

	XMFLOAT3 mColor;
};

#endif