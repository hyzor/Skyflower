#include "AnimatedEntity.h"

// Must be included last!
#include "shared/debug.h"

AnimatedEntity::AnimatedEntity(GenericSkinnedModel* model, XMFLOAT3 position)
{
	//mInstance.model = model;
	mInstance.isVisible = true;
	mInstance.TimePos = 0.0f;
	mInstance.AnimationName = "animation";
	mInstance.AnimationIndex = mInstance.model->skinnedData.GetAnimationIndex(mInstance.AnimationName);
	mInstance.FinalTransforms.resize(mInstance.model->skinnedData.Bones.size());
	mInstance.frameStart = 51+15;
	mInstance.frameEnd = 51+15;
	this->Position = position;
	this->Scale = XMFLOAT3(1, 1, 1);
	this->Rotation = 0.0f;
	mFirstAnimation = true;

	SetPosition(position);
}


AnimatedEntity::~AnimatedEntity(void)
{
}

// void AnimatedEntity::Draw( ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, Camera* mCamera)
// {
// 	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
// 	dc->IASetInputLayout(InputLayouts::PosNormalTexTanSkinned);
// 
// 	UINT stride = sizeof(Vertex::PosNormalTexTanSkinned);
// 	UINT offset = 0;
// 
// 	XMMATRIX world;
// 	XMMATRIX worldInvTranspose;
// 	XMMATRIX worldViewProj;
// 
// 	XMMATRIX view = mCamera->GetViewMatrix();
// 	XMMATRIX proj = mCamera->GetProjMatrix();
// 	XMMATRIX viewproj = mCamera->GetViewProjMatrix();
// 
// 	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
// 	XMMATRIX toTexSpace(
// 		0.5f, 0.0f, 0.0f, 0.0f,
// 		0.0f, -0.5f, 0.0f, 0.0f,
// 		0.0f, 0.0f, 1.0f, 0.0f,
// 		0.5f, 0.5f, 0.0f, 1.0f);
// 
// 	D3DX11_TECHNIQUE_DESC techDesc;
// 	activeTech->GetDesc(&techDesc);
// 
// 	world = XMLoadFloat4x4(&mInstance.world);
// 	worldInvTranspose = MathHelper::InverseTranspose(world);
// 	worldViewProj = world*view*proj;
// 
// 	Effects::NormalMapFX->SetWorld(world);
// 	Effects::NormalMapFX->SetWorldInvTranspose(worldInvTranspose);
// 	Effects::NormalMapFX->SetWorldViewProj(worldViewProj);
// 	Effects::NormalMapFX->SetWorldViewProjTex(worldViewProj*toTexSpace);
// 	//Effects::NormalMapFX->SetShadowTransform(world*XMLoadFloat4x4(&shadowMap->GetShadowTransform()));
// 	//Effects::NormalMapFX->SetShadowMap(shadowMap->getDepthMapSRV());
// 	Effects::NormalMapFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));
// 	//Effects::NormalMapFX->SetBoneTransforms(mInstance.model->skinnedData.Transforms, )
// 	Effects::NormalMapFX->SetBoneTransforms(&mInstance.FinalTransforms[0], mInstance.FinalTransforms.size());
// 
// 	for (UINT p = 0; p < techDesc.Passes; ++p)
// 	{
// 		for (UINT i = 0; i < mInstance.model->numMeshes; ++i)
// 		{
// 			UINT matIndex = mInstance.model->meshes[i].mMaterialIndex;
// 
// 			Effects::NormalMapFX->SetMaterial(mInstance.model->mat[matIndex]);
// 
// 			Effects::NormalMapFX->SetDiffuseMap(mInstance.model->diffuseMapSRV[matIndex]);
// 
// 			activeTech->GetPassByIndex(p)->Apply(0, dc);
// 			mInstance.model->meshes[i].draw(dc);
// 		}
// 	}
// }

void AnimatedEntity::Update(float dt)
{
	mInstance.Update(dt);
}

void AnimatedEntity::SetPosition(XMFLOAT3 pos)
{
	this->Position = pos;

	XMMATRIX modelScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	XMMATRIX modelRot = XMMatrixRotationY(0.0f);
	XMMATRIX modelOffset = XMMatrixTranslation(Position.x, Position.y, Position.z);
	XMStoreFloat4x4(&mInstance.world, modelScale*modelRot*modelOffset);
}

void AnimatedEntity::RotateXYZ(XMFLOAT3 rot, float yaw, XMVECTOR Up)
{
	XMMATRIX modelRot = XMMatrixRotationX(rot.x) * XMMatrixRotationY(rot.y) * XMMatrixRotationZ(rot.z) * XMMatrixRotationAxis(Up, yaw);
	XMMATRIX modelScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	XMMATRIX modelOffset = XMMatrixTranslation(Position.x, Position.y, Position.z);
	XMStoreFloat4x4(&mInstance.world, modelScale*modelRot*modelOffset);
}

void AnimatedEntity::SetAnimation(UINT index, bool loop)
{
	if (index == mCurAnim && !mFirstAnimation)
		return;

	//mInstance.AnimationIndex = index;
	mInstance.TimePos = 0.0f;
	mInstance.frameStart = mAnimations[index].FrameStart;
	mInstance.frameEnd = mAnimations[index].FrameEnd;
	mInstance.loop = loop;
	mInstance.animationDone = false;

	if (mAnimations[index].playForwards)
		mInstance.playAnimForward = true;
	else
		mInstance.playAnimForward = false;
	
	mCurAnim = index;
	mFirstAnimation = false;
}

void AnimatedEntity::Draw(ID3D11DeviceContext* dc, Camera* cam, NormalMappedSkinned* shader, XMMATRIX &world)
{
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	dc->IASetInputLayout(InputLayouts::PosNormalTexTanSkinned);

	XMMATRIX toTexSpace(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	shader->SetWorldViewProjTex(dc, world, cam->GetViewProjMatrix(), toTexSpace);
	shader->SetBoneTransforms(dc, mInstance.FinalTransforms.data(), (UINT)mInstance.FinalTransforms.size());

	for (UINT i = 0; i < mInstance.model->numMeshes; ++i)
	{
		UINT matIndex = mInstance.model->meshes[i].mMaterialIndex;

		shader->SetMaterial(dc, mInstance.model->mat[matIndex]);
		shader->SetDiffuseMap(dc, mInstance.model->diffuseMapSRV[matIndex].Get());
		shader->SetNormalMap(dc, mInstance.model->normalMapSRV[matIndex].Get());
		shader->UpdatePerObj(dc);

		mInstance.model->meshes[i].draw(dc);
	}
}

void AnimatedEntity::Draw(ID3D11DeviceContext* dc, Camera* cam, BasicDeferredSkinnedShader* deferredShader, XMMATRIX &world)
{
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//dc->IASetInputLayout(InputLayouts::PosNormalTexTanSkinned);

	XMMATRIX toTexSpace(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	deferredShader->SetWorldViewProjTex(world, cam->GetViewProjMatrix(), toTexSpace);
	deferredShader->SetBoneTransforms(mInstance.FinalTransforms.data(), (UINT)mInstance.FinalTransforms.size());

	for (UINT i = 0; i < mInstance.model->numMeshes; ++i)
	{
		UINT matIndex = mInstance.model->meshes[i].mMaterialIndex;

		deferredShader->SetMaterial(mInstance.model->mat[matIndex], mInstance.model->mGlobalMaterialIndex[matIndex]);
		deferredShader->SetDiffuseMap(dc, mInstance.model->diffuseMapSRV[matIndex].Get());
		//shader->SetNormalMap(dc, mInstance.model->normalMapSRV[matIndex]);
		deferredShader->UpdatePerObj(dc);

		mInstance.model->meshes[i].draw(dc);
	}
}

void AnimatedEntity::SetAnimationSpeed(UINT index, float speed)
{
	this->mAnimations[index].AnimationSpeed = speed;
}