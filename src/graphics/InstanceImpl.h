#ifndef INSTANCEIMPL_H
#define INSTANCEIMPL_H

#include "shared/platform.h"
#include "GenericModel.h"
#include "AnimatedEntity.h"
#include "Instance.h"


class ModelInstanceImpl : public ModelInstance
{
public:
	ModelInstanceImpl(Vec3 pos, Vec3 rot, Vec3 scale);
	~ModelInstanceImpl();

	bool IsVisible();
	Vec3 GetPosition();
	Vec3 GetRotation();
	Vec3 GetScale();

	void SetVisibility(bool visible);
	void SetPosition(Vec3 pos);
	void SetRotation(Vec3 rot);
	void SetScale(Vec3 scale);
	void Set(Vec3 pos, Vec3 rot, Vec3 scale);

	void SetPrevWorld(XMMATRIX& prevWorld);
	XMMATRIX GetPrevWorld();

	XMMATRIX GetWorld();

	GenericModel* model;

private:
	XMFLOAT4X4 modelOffset;
	XMFLOAT4X4 modelRot;
	XMFLOAT4X4 modelScale;
	XMFLOAT4X4 modelWorld;
	XMFLOAT4X4 mPrevWorld; // World matrix from previous frame

	bool isVisible;
	Vec3 pos;
	Vec3 rot;
	Vec3 scale;

};

class AnimatedInstanceImpl : public AnimatedInstance
{
public:
	AnimatedInstanceImpl(Vec3 pos, Vec3 rot, Vec3 scale);
	~AnimatedInstanceImpl();

	bool IsVisible();
	Vec3 GetPosition();
	Vec3 GetRotation();
	Vec3 GetScale();
	int GetAnimation();

	void SetVisibility(bool visible);
	void SetPosition(Vec3 pos);
	void SetRotation(Vec3 rot);
	void SetScale(Vec3 scale);
	void Set(Vec3 pos, Vec3 rot, Vec3 scale);

	void CreateAnimation(int id, int start, int frames);
	void CreateAnimation(int id, int start, int frames, bool playForwards);
	void SetAnimation(int id);

	XMMATRIX GetWorld();

	AnimatedEntity* model;

private:
	XMFLOAT4X4 modelOffset;
	XMFLOAT4X4 modelRot;
	XMFLOAT4X4 modelScale;
	XMFLOAT4X4 modelWorld;

	bool isVisible;
	Vec3 pos;
	Vec3 rot;
	Vec3 scale;

};



#endif