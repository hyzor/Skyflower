#include "ParticleSystem.h"


ParticleSystem::ParticleSystem()
{
}


ParticleSystem::~ParticleSystem()
{
}

float ParticleSystem::GetAge() const
{

}

void ParticleSystem::SetEyePos(const XMFLOAT3& eyePosW)
{

}

void ParticleSystem::SetEmitPos(const XMFLOAT3& emitPosW)
{

}

void ParticleSystem::SetEmitDir(const XMFLOAT3& emitDirW)
{

}

void ParticleSystem::Init(ID3D11Device* device, IShader* shader, ID3D11ShaderResourceView* texArraySRV, ID3D11ShaderResourceView* randomTexSRV, UINT maxParticles)
{

}

void ParticleSystem::Reset()
{

}

void ParticleSystem::Update(float dt)
{

}

void ParticleSystem::Draw(ID3D11DeviceContext* dc, const Camera& cam)
{
	// Set shader constants

	dc->IASetInputLayout(InputLayouts::Particle);

}

void ParticleSystem::BuildVB(ID3D11Device* device)
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
	p.Type = 0;

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
