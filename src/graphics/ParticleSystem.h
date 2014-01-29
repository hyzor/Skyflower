#ifndef PARTICLESYSTEM_H_
#define PARTICLESYSTEM_H_

#include "ShaderHandler.h"
#include "Camera.h"
#include "Vertex.h"

using namespace DirectX;

// Stream-out declaration entries
class SoDeclarationEntry
{
public:
	static D3D11_SO_DECLARATION_ENTRY ParticleSoDesc[5];
	static UINT stride;
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

	void Init(ID3D11Device* device, ParticleSystemShader* shader,
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
};

#endif