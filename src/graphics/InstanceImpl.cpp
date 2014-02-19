#include "InstanceImpl.h"

// Must be included last!
#include "shared/debug.h"

ModelInstanceImpl::ModelInstanceImpl(Vec3 pos, Vec3 rot, Vec3 scale)
{
	this->isVisible = true;
	this->type = 0;
	Set(pos, rot, scale);
}

ModelInstanceImpl::~ModelInstanceImpl()
{
}

void ModelInstanceImpl::SetPosition(Vec3 pos)
{
	this->pos = pos;

	XMMATRIX offset = XMMatrixTranslation(pos.X, pos.Y, pos.Z);
	XMMATRIX rot = XMLoadFloat4x4(&modelRot);
	XMMATRIX scale = XMLoadFloat4x4(&modelScale);

	XMMATRIX world = scale*rot*offset;

	XMStoreFloat4x4(&modelOffset, offset);
	XMStoreFloat4x4(&modelWorld, world);
}

void ModelInstanceImpl::SetRotation(Vec3 rot)
{
	this->rot = rot;

	XMMATRIX offset = XMLoadFloat4x4(&modelOffset);
	XMMATRIX scale = XMLoadFloat4x4(&modelScale);

	XMMATRIX mrot = XMMatrixRotationX(rot.X);
	mrot *= XMMatrixRotationY(rot.Y);
	mrot *= XMMatrixRotationZ(rot.Z);

	XMMATRIX world = scale*mrot*offset;

	XMStoreFloat4x4(&modelRot, mrot);
	XMStoreFloat4x4(&modelWorld, world);
}

void ModelInstanceImpl::SetScale(Vec3 scale)
{
	this->scale = scale;

	XMMATRIX offset = XMLoadFloat4x4(&modelOffset);
	XMMATRIX rot = XMLoadFloat4x4(&modelRot);
	XMMATRIX mscale = XMMatrixScaling(scale.X, scale.Y, scale.Z);

	XMMATRIX world = mscale*rot*offset;

	XMStoreFloat4x4(&modelScale, mscale);
	XMStoreFloat4x4(&modelWorld, world);
}

void ModelInstanceImpl::Set(Vec3 pos, Vec3 rot, Vec3 scale)
{
	this->pos = pos;
	this->rot = rot;
	this->scale = scale;

	XMMATRIX offset = XMMatrixTranslation(pos.X, pos.Y, pos.Z);
	XMMATRIX mrot = XMMatrixRotationX(rot.X);
	mrot *= XMMatrixRotationY(rot.Y);
	mrot *= XMMatrixRotationZ(rot.Z);
	XMMATRIX mscale = XMMatrixScaling(scale.X, scale.Y, scale.Z);

	XMMATRIX world = mscale*mrot*offset;

	XMStoreFloat4x4(&modelOffset, offset);
	XMStoreFloat4x4(&modelRot, mrot);
	XMStoreFloat4x4(&modelScale, mscale);
	XMStoreFloat4x4(&modelWorld, world);
	XMStoreFloat4x4(&mPrevWorld, world);
}

void ModelInstanceImpl::SetVisibility(bool visible)
{
	isVisible = visible;
}
bool ModelInstanceImpl::IsVisible()
{
	return isVisible;
}
Vec3 ModelInstanceImpl::GetPosition()
{
	return pos;
}
Vec3 ModelInstanceImpl::GetRotation()
{
	return rot;
}
Vec3 ModelInstanceImpl::GetScale()
{
	return scale;
}

XMMATRIX ModelInstanceImpl::GetWorld()
{
	return XMLoadFloat4x4(&modelWorld);
}

int ModelInstanceImpl::GetType()
{
	return this->type;
}

void ModelInstanceImpl::SetType(int type)
{
	this->type = type;
}
void ModelInstanceImpl::SetPrevWorld(XMMATRIX& prevWorld)
{
	XMStoreFloat4x4(&mPrevWorld, prevWorld);
}

XMMATRIX ModelInstanceImpl::GetPrevWorld()
{
	return XMLoadFloat4x4(&mPrevWorld);
}



AnimatedInstanceImpl::AnimatedInstanceImpl(Vec3 pos, Vec3 rot, Vec3 scale)
{
	this->isVisible = true;
	Set(pos, rot, scale);

	
}

AnimatedInstanceImpl::~AnimatedInstanceImpl()
{
}

void AnimatedInstanceImpl::SetPosition(Vec3 pos)
{
	this->pos = pos;

	XMMATRIX offset = XMMatrixTranslation(pos.X, pos.Y, pos.Z);
	XMMATRIX rot = XMLoadFloat4x4(&modelRot);
	XMMATRIX scale = XMLoadFloat4x4(&modelScale);

	XMMATRIX world = scale*rot*offset;

	XMStoreFloat4x4(&modelOffset, offset);
	XMStoreFloat4x4(&modelWorld, world);
}

void AnimatedInstanceImpl::SetRotation(Vec3 rot)
{
	this->rot = rot;

	XMMATRIX offset = XMLoadFloat4x4(&modelOffset);
	XMMATRIX scale = XMLoadFloat4x4(&modelScale);

	XMMATRIX mrot = XMMatrixRotationX(rot.X);
	mrot *= XMMatrixRotationY(rot.Y);
	mrot *= XMMatrixRotationZ(rot.Z);

	XMMATRIX world = scale*mrot*offset;

	XMStoreFloat4x4(&modelRot, mrot);
	XMStoreFloat4x4(&modelWorld, world);
}

void AnimatedInstanceImpl::SetScale(Vec3 scale)
{
	this->scale = scale;

	XMMATRIX offset = XMLoadFloat4x4(&modelOffset);
	XMMATRIX rot = XMLoadFloat4x4(&modelRot);
	XMMATRIX mscale = XMMatrixScaling(scale.X, scale.Y, scale.Z);

	XMMATRIX world = mscale*rot*offset;

	XMStoreFloat4x4(&modelScale, mscale);
	XMStoreFloat4x4(&modelWorld, world);
}

void AnimatedInstanceImpl::Set(Vec3 pos, Vec3 rot, Vec3 scale)
{
	this->pos = pos;
	this->rot = rot;
	this->scale = scale;

	XMMATRIX offset = XMMatrixTranslation(pos.X, pos.Y, pos.Z);
	XMMATRIX mrot = XMMatrixRotationX(rot.X);
	mrot *= XMMatrixRotationY(rot.Y);
	mrot *= XMMatrixRotationZ(rot.Z);
	XMMATRIX mscale = XMMatrixScaling(scale.X, scale.Y, scale.Z);

	XMMATRIX world = mscale*mrot*offset;

	XMStoreFloat4x4(&modelOffset, offset);
	XMStoreFloat4x4(&modelRot, mrot);
	XMStoreFloat4x4(&modelScale, mscale);
	XMStoreFloat4x4(&modelWorld, world);
}

void AnimatedInstanceImpl::SetVisibility(bool visible)
{
	isVisible = visible;
}
bool AnimatedInstanceImpl::IsVisible()
{
	return isVisible;
}
Vec3 AnimatedInstanceImpl::GetPosition()
{
	return pos;
}
Vec3 AnimatedInstanceImpl::GetRotation()
{
	return rot;
}
Vec3 AnimatedInstanceImpl::GetScale()
{
	return scale;
}

UINT AnimatedInstanceImpl::GetAnimation()
{
	return model->mCurAnim;
}

bool AnimatedInstanceImpl::IsAnimationDone()
{
	return model->mInstance.animationDone;
}

XMMATRIX AnimatedInstanceImpl::GetWorld()
{
	return XMLoadFloat4x4(&modelWorld);
}


void AnimatedInstanceImpl::CreateAnimation(int id, int start, int frames)
{
	model->mAnimations.push_back(AnimatedEntity::Animation(id, start, frames));
}

void AnimatedInstanceImpl::CreateAnimation(int id, int start, int frames, bool playForwards)
{
	model->mAnimations.push_back(AnimatedEntity::Animation(id, start, frames, playForwards));
}

void AnimatedInstanceImpl::SetAnimation(UINT id, bool loop)
{
	model->SetAnimation(id, loop);
}

void AnimatedInstanceImpl::SetPrevWorld(XMMATRIX& prevWorld)
{
	XMStoreFloat4x4(&mPrevWorld, prevWorld);
}

DirectX::XMMATRIX AnimatedInstanceImpl::GetPrevWorld()
{
	return XMLoadFloat4x4(&mPrevWorld);
}

void AnimatedInstanceImpl::Update(float deltaTime)
{
	model->Update(deltaTime);
}


void MorphModelInstanceImpl::SetPosition(Vec3 pos)
{
	this->pos = pos;

	XMMATRIX offset = XMMatrixTranslation(pos.X, pos.Y, pos.Z);
	XMMATRIX rotation = XMMatrixRotationY(this->rot.Y);
	XMMATRIX scaling = XMMatrixScaling(scale.X, scale.Y, scale.Z);

	XMMATRIX w = scaling*rotation*offset;

	XMStoreFloat4x4(&world, w);
}

void MorphModelInstanceImpl::SetRotation(Vec3 rot)
{
	this->rot = rot;

	XMMATRIX offset = XMMatrixTranslation(pos.X, pos.Y, pos.Z);
	XMMATRIX scaling = XMMatrixScaling(scale.X, scale.Y, scale.Z);

	XMMATRIX mrot = XMMatrixRotationX(rot.X);
	mrot *= XMMatrixRotationY(rot.Y);
	mrot *= XMMatrixRotationZ(rot.Z);

	XMMATRIX w = scaling*mrot*offset;

	XMStoreFloat4x4(&world, w);
}

void MorphModelInstanceImpl::SetScale(Vec3 scale)
{
	this->scale = scale;

	XMMATRIX offset = XMMatrixTranslation(pos.X, pos.Y, pos.Z);
	XMMATRIX rotation = XMMatrixRotationY(this->rot.Y);
	XMMATRIX mscale = XMMatrixScaling(scale.X, scale.Y, scale.Z);

	XMMATRIX w = mscale*rotation*offset;

	XMStoreFloat4x4(&world, w);
}

void MorphModelInstanceImpl::SetVisibility(bool visible)
{
	isVisible = visible;
}

void MorphModelInstanceImpl::SetWeights(Vec3 weights)
{
	this->weights.x = weights.X;
	this->weights.y = weights.Y;
	this->weights.z = weights.Z;
}

bool MorphModelInstanceImpl::IsVisible() const
{
	return isVisible;
}
Vec3 MorphModelInstanceImpl::GetPosition() const
{
	return pos;
}
Vec3 MorphModelInstanceImpl::GetRotation() const
{
	return rot;
}
Vec3 MorphModelInstanceImpl::GetScale() const
{
	return scale;
}

Vec3 MorphModelInstanceImpl::GetWeights() const
{
	return Vec3(weights.x, weights.y, weights.z);
}

void MorphModelInstanceImpl::Set(Vec3 pos, Vec3 rot, Vec3 scale, Vec3 weights)
{
	this->pos = pos;
	this->rot = rot;
	this->scale = scale;
	this->weights = XMFLOAT4(weights.X, weights.Y, weights.Z, 0.0f);

	XMMATRIX offset = XMMatrixTranslation(pos.X, pos.Y, pos.Z);
	XMMATRIX mrot = XMMatrixRotationX(rot.X);
	mrot *= XMMatrixRotationY(rot.Y);
	mrot *= XMMatrixRotationZ(rot.Z);
	XMMATRIX mscale = XMMatrixScaling(scale.X, scale.Y, scale.Z);

	XMMATRIX w = mscale*mrot*offset;

	XMStoreFloat4x4(&world, w);
}