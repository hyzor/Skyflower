#ifndef PARTICLESYSTEM_H_
#define PARTICLESYSTEM_H_

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

// Must be mirrored with "ParticleSystemShared.hlsli"
static const enum ParticleType
{
	PT_EMITTER,
	PT_FLARE0,
	PT_FLARE1,
	PT_PARTICLE,
	NROFTYPES
};

class ParticleSystem
{
public:
	ParticleSystem();
	~ParticleSystem();

	float GetAge() const;

	void SetEyePos(const XMFLOAT3& eyePosW);
	void SetEmitPos(const XMFLOAT3& emitPosW);
	void SetEmitDir(const XMFLOAT3& emitDirW);

	void SetConstantAccel(XMFLOAT3 accelW);

	void SetEmitFrequency(float emitFrequency);
	void SetParticleAgeLimit(float particleAgeLimit);

	void SetParticleType(UINT particleType);

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
	UINT mMaxParticles;
	bool mFirstRun;

	float mTimeStep;
	float mGameTime;
	float mAge;

	XMFLOAT3 mEyePosW;
	XMFLOAT3 mEmitPosW;
	XMFLOAT3 mEmitDirW;

	XMFLOAT3 mConstantAccelW;

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
};

#endif