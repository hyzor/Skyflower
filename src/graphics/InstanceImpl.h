#ifndef INSTANCEIMPL_H
#define INSTANCEIMPL_H

#include "shared/platform.h"
#include "GenericModel.h"
#include "AnimatedEntity.h"
#include "Instance.h"
#include "MorphModel.h"

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
	int GetType();
	void SetType(int type);

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
	int type;
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
	UINT GetAnimation();

	void SetVisibility(bool visible);
	void SetPosition(Vec3 pos);
	void SetRotation(Vec3 rot);
	void SetScale(Vec3 scale);
	void Set(Vec3 pos, Vec3 rot, Vec3 scale);

	void SetPrevWorld(XMMATRIX& prevWorld);
	XMMATRIX GetPrevWorld();

	void CreateAnimation(int id, int start, int frames);
	void CreateAnimation(int id, int start, int frames, bool playForwards);
	void SetAnimation(UINT id, bool loop);

	XMMATRIX GetWorld();

	void Update(float deltaTime);

	AnimatedEntity* model;

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

class MorphModelInstanceImpl : public MorphModelInstance
{
public:
	 ~MorphModelInstanceImpl() {};

	 bool IsVisible() const;
	 Vec3 GetPosition() const;
	 Vec3 GetRotation() const;
	 Vec3 GetScale() const;
	 Vec3 GetWeights() const;

	 void SetPosition(Vec3 pos);
	 void SetRotation(Vec3 rot);
	 void SetScale(Vec3 scale);
	 void SetVisibility(bool visible);
	 void SetWeights(Vec3 weights);
	 void Set(Vec3 pos, Vec3 rot, Vec3 scale, Vec3 weights);

	 MorphModel* model;

	 XMFLOAT4X4 world;
	 bool isVisible;
	 XMFLOAT4 weights;
	 XMFLOAT4X4 prevWorld;
private:
	Vec3 pos;
	Vec3 rot;
	Vec3 scale;
};

#endif