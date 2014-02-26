#include "ParticleSystemImpl.h"

// Must be included last!
#include "shared/debug.h"

D3D11_SO_DECLARATION_ENTRY GeoStreamOutDesc::ParticleSoDesc[5] =
{
	// stream number, semantic name, semantic index, start component, component count, output slot
	{ 0, "POSITION", 0, 0, 3, 0 },	// output the first 3 of position
	{ 0, "VELOCITY", 0, 0, 3, 0 }, // output the first 3 of velocity
	{ 0, "SIZE", 0, 0, 2, 0 },		// output the first 2 of size
	{ 0, "AGE", 0, 0, 1, 0 },		// output only the first from age
	{ 0, "TYPE", 0, 0, 1, 0 },		// output only the first from type
};

UINT GeoStreamOutDesc::ParticleStride = sizeof(Vertex::Particle);

ParticleSystemImpl::ParticleSystemImpl()
: mInitVB(0), mDrawVB(0), mStreamOutVB(0), mTexArraySRV(0), mRandomTexSRV(0)
{
	mActive = true;
	mFirstRun = true;
	mGameTime = 0.0f;
	mTimeStep = 0.0f;
	mAge = 0.0f;

	mEyePosW = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mEmitPosW = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mEmitDirW = XMFLOAT3(0.0f, 1.0f, 0.0f);

	mEmitFrequency = 0.005f;
	mParticleAgeLimit = 1.0f;
	mParticleType = ParticleType::PT_FLARE0;

	mFadeTime = 1.0f;
	mBlendingMethod = 0;

	mScale = XMFLOAT2(1.0f, 1.0f);

	mRandomizeVelocity = true;
	mRandomVelocity = XMFLOAT3(4.0f, 4.0f, 4.0f);
}

ParticleSystemImpl::~ParticleSystemImpl()
{
	ReleaseCOM(mInitVB);
	ReleaseCOM(mDrawVB);
	ReleaseCOM(mStreamOutVB);
}

void ParticleSystemImpl::SetActive(bool active)
{
	mActive = active;
}

bool ParticleSystemImpl::IsActive() const
{
	return mActive;
}

float ParticleSystemImpl::GetAge() const
{
	return mAge;
}

float ParticleSystemImpl::GetAgeLimit() const
{
	return mParticleAgeLimit;
}

void ParticleSystemImpl::SetEyePos(const XMFLOAT3& eyePosW)
{
	mEyePosW = eyePosW;
}

void ParticleSystemImpl::SetEmitPos(const XMFLOAT3& emitPosW)
{
	mEmitPosW = emitPosW;
}

void ParticleSystemImpl::SetEmitDir(const XMFLOAT3& emitDirW)
{
	mEmitDirW = emitDirW;
}

void ParticleSystemImpl::Init(ID3D11Device* device,
	ParticleSystemShader* shader,
	ID3D11ShaderResourceView* texArraySRV,
	ID3D11ShaderResourceView* randomTexSRV,
	UINT maxParticles)
{
	mShader = shader;
	mTexArraySRV = texArraySRV;
	mRandomTexSRV = randomTexSRV;
	mMaxParticles = maxParticles;

	BuildVB(device);
}

void ParticleSystemImpl::Reset()
{
	mFirstRun = true;
	mAge = 0.0f;
}

void ParticleSystemImpl::Update(float dt, float gameTime)
{
	mTimeStep = dt;
	mGameTime = gameTime;

	mAge += dt;
}

void ParticleSystemImpl::Draw(ID3D11DeviceContext* dc, const Camera& cam)
{
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	// Disable depth buffer for stream-out only
	dc->OMSetDepthStencilState(RenderStates::mDisabledDSS, 1);

	// Set shader constants
	//mShader->SetActive(dc);
	mShader->SetEmitProperties(mEmitPosW, mEmitDirW);
	mShader->SetEyePosW(mEyePosW);
	mShader->SetTexArray(dc, mTexArraySRV);
	mShader->SetRandomTex(dc, mRandomTexSRV);
	mShader->SetTime(mGameTime, mTimeStep);
	mShader->SetViewProj(cam.GetViewProjMatrix(), cam.GetViewMatrix());
	mShader->SetPrevViewProj(cam.GetPreviousViewProj());
	mShader->SetAccelConstant(mConstantAccelW);
	mShader->SetParticleProperties(mParticleAgeLimit, mEmitFrequency);
	mShader->SetParticleType(mParticleType);
	mShader->SetEmitParticles(mActive);
	mShader->SetParticleFadeTime(mFadeTime);
	mShader->SetFarNearClipDistance(cam.GetFarZ(), cam.GetNearZ());

	mShader->SetBlendingMethod(mBlendingMethod);
	mShader->SetScale(mScale.x, mScale.y);

	mShader->RandomizeVelocity(mRandomizeVelocity);
	mShader->SetRandomVelocity(mRandomVelocity);

	mShader->ActivateStreamShaders(dc);
	mShader->UpdateStreamOutShaders(dc);

	//UINT stride = sizeof(Vertex::Particle);
	UINT offset = 0;

	if (mFirstRun)
		dc->IASetVertexBuffers(0, 1, &mInitVB, &GeoStreamOutDesc::ParticleStride, &offset);
	else
		dc->IASetVertexBuffers(0, 1, &mDrawVB, &GeoStreamOutDesc::ParticleStride, &offset);

	// First draw current particle list using stream-out only
	dc->SOSetTargets(1, &mStreamOutVB, &offset);
	// Use StreamOutParticleVS and StreamOutParticleGS

	if (mFirstRun)
	{
		dc->Draw(1, 0);
		mFirstRun = false;
	}
	else
	{
		dc->DrawAuto();
	}

	// Streaming-out done, unbind vertex buffer
	ID3D11Buffer* bufferArray[1] = { 0 };
	dc->SOSetTargets(1, bufferArray, &offset);

	// Swap draw vertex buffer with stream-out vertex buffer
	std::swap(mDrawVB, mStreamOutVB);

	// Restore depth buffer
	dc->OMSetDepthStencilState(RenderStates::mDepthStencilEnabledDSS, 1);
	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	//dc->OMSetBlendState(RenderStates::mAdditiveBS, blendFactor, 0xffffffff);
	dc->OMSetBlendState(RenderStates::mDefaultBS, blendFactor, 0xffffffff);

	// Now draw the update particle system we just streamed out
	// Use DrawParticleVS, DrawParticleGS and DrawParticlePS
	mShader->ActivateDrawShaders(dc);
	mShader->UpdateDrawShaders(dc);

	dc->IASetVertexBuffers(0, 1, &mDrawVB, &GeoStreamOutDesc::ParticleStride, &offset);
	dc->DrawAuto();
}

void ParticleSystemImpl::BuildVB(ID3D11Device* device)
{
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = sizeof(Vertex::Particle) * 1;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	// The initial particle emitter has type 0 and age 0.  The rest
	// of the particle attributes do not apply to an emitter.
	Vertex::Particle p;
	ZeroMemory(&p, sizeof(Vertex::Particle));
	p.Age = 0.0f;
	p.Type = ParticleType::PT_EMITTER;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &p;

	HR(device->CreateBuffer(&vbd, &vinitData, &mInitVB));

	//
	// Create the ping-pong buffers for stream-out and drawing.
	//
	vbd.ByteWidth = sizeof(Vertex::Particle) * mMaxParticles;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;

	HR(device->CreateBuffer(&vbd, 0, &mDrawVB));
	HR(device->CreateBuffer(&vbd, 0, &mStreamOutVB));
}

void ParticleSystemImpl::SetConstantAccel(XMFLOAT3 accelW)
{
	mConstantAccelW = accelW;
}

void ParticleSystemImpl::SetEmitFrequency(float emitFrequency)
{
	mEmitFrequency = emitFrequency;
}

void ParticleSystemImpl::SetParticleAgeLimit(float particleAgeLimit)
{
	mParticleAgeLimit = particleAgeLimit;
}

void ParticleSystemImpl::SetParticleType(ParticleType particleType)
{
	if (particleType < ParticleType::NROFTYPES)
		mParticleType = particleType;
	else
		particleType = ParticleType::PT_FLARE0;
}

void ParticleSystemImpl::SetParticleFadeTime(float fadeTime)
{
	mFadeTime = fadeTime;
}

void ParticleSystemImpl::SetBlendingMethod(unsigned int blendingMethod)
{
	mBlendingMethod = blendingMethod;
}

void ParticleSystemImpl::SetScale(XMFLOAT2 scale)
{
	mScale = scale;
}

void ParticleSystemImpl::SetRandomVelocityActive(bool active)
{
	mRandomizeVelocity = active;
}

void ParticleSystemImpl::SetRandomVelocity(XMFLOAT3 randomVelocity)
{
	mRandomVelocity = randomVelocity;
}
