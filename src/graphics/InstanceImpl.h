#ifndef INSTANCEIMPL_H
#define INSTANCEIMPL_H

#include "shared/platform.h"
#include "GenericModel.h"
#include "GenericSkinnedModelSorted.h"
//#include "AnimatedEntity.h"
#include "Instance.h"
#include "MorphModel.h"
#include "Camera.h"

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
	//AnimatedInstanceImpl(Vec3 pos, Vec3 rot, Vec3 scale);
	AnimatedInstanceImpl(Vec3 pos, Vec3 rot, Vec3 scale, GenericSkinnedModel* model);
	~AnimatedInstanceImpl();

	bool IsVisible();
	Vec3 GetPosition();
	Vec3 GetRotation();
	Vec3 GetScale();
	UINT GetAnimation();
	bool IsAnimationDone();

	void SetVisibility(bool visible);
	void SetPosition(Vec3 pos);
	void SetRotation(Vec3 rot);
	void SetScale(Vec3 scale);
	void Set(Vec3 pos, Vec3 rot, Vec3 scale);
	void SetAnimationSpeed(UINT id, float speed);
	void SetPrevWorld(XMMATRIX& prevWorld);
	XMMATRIX GetPrevWorld();

	void CreateAnimation(int id, int start, int frames);
	void CreateAnimation(int id, int start, int frames, bool playForwards);
	void SetAnimation(UINT index, bool loop);

	void Draw(ID3D11DeviceContext* dc, Camera* cam, BasicDeferredSkinnedShader* deferredShader);

	XMMATRIX GetWorld();

	void Update(float deltaTime);

	void SetModel(GenericSkinnedModel* model);

	//AnimatedEntity* model;

	//GenericSkinnedModel* mModel;
	GenericSkinnedModelInstance* mSkinnedInstance;

private:
	XMFLOAT4X4 modelOffset;
	XMFLOAT4X4 modelRot;
	XMFLOAT4X4 modelScale;
	XMFLOAT4X4 modelWorld;
	XMFLOAT4X4 mPrevWorld; // World matrix from previous frame

	bool mIsVisible;
	Vec3 mPos;
	Vec3 mRot;
	Vec3 mScale;

	UINT mCurAnim;

	struct Animation
	{
		Animation(UINT animationType, UINT frameStart, UINT frameEnd, float animationSpeed)
		{
			this->AnimationType = animationType;
			this->FrameStart = frameStart;
			this->FrameEnd = frameEnd;
			this->playForwards = true;
			this->AnimationSpeed = animationSpeed;
		}

		Animation(UINT animationType, UINT frameStart, UINT frameEnd, bool playForwards, float animationSpeed)
		{
			this->AnimationType = animationType;
			this->FrameStart = frameStart;
			this->FrameEnd = frameEnd;
			this->playForwards = playForwards;
			this->AnimationSpeed = animationSpeed;
		}

		float AnimationSpeed;
		UINT FrameStart, FrameEnd;
		UINT AnimationType;
		bool playForwards;
	};

	std::vector<Animation> mAnimations;

	bool mFirstAnimation;
};

class SortedAnimatedInstanceImpl : public SortedAnimatedInstance
{
public:
	//AnimatedInstanceImpl(Vec3 pos, Vec3 rot, Vec3 scale);
	SortedAnimatedInstanceImpl(Vec3 pos, Vec3 rot, Vec3 scale, GenericSkinnedModelSorted* model);
	~SortedAnimatedInstanceImpl();

	bool IsVisible();
	Vec3 GetPosition();
	Vec3 GetRotation();
	Vec3 GetScale();
	UINT GetLowerAnimation();
	//bool IsLowerAnimationDone();
	UINT GetUpperAnimation();
	//bool IsUpperAnimationDone();

	void SetVisibility(bool visible);
	void SetPosition(Vec3 pos);
	void SetRotation(Vec3 rot);
	void SetScale(Vec3 scale);
	void Set(Vec3 pos, Vec3 rot, Vec3 scale);
	void SetPrevWorld(XMMATRIX& prevWorld);
	XMMATRIX GetPrevWorld();

	void CreateAnimation(int id, int start, int frames);
	void CreateAnimation(int id, int start, int frames, bool playForwards);

	void SetLowerAnimation(UINT index, bool loop);
	//void SetLowerAnimationSpeed(UINT id, float speed);
	void SetUpperAnimation(UINT index, bool loop);
	//void SetUpperAnimationSpeed(UINT id, float speed);

	void Draw(ID3D11DeviceContext* dc, Camera* cam, BasicDeferredSkinnedSortedShader* deferredShader);

	XMMATRIX GetWorld();

	void Update(float deltaTime);

	void SetModel(GenericSkinnedModelSorted* model);

	GenericSkinnedModelSortedInstance* mSkinnedInstance;

private:
	XMFLOAT4X4 modelOffset;
	XMFLOAT4X4 modelRot;
	XMFLOAT4X4 modelScale;
	XMFLOAT4X4 modelWorld;
	XMFLOAT4X4 mPrevWorld; // World matrix from previous frame

	bool mIsVisible;
	Vec3 mPos;
	Vec3 mRot;
	Vec3 mScale;

	UINT mCurLowerAnim;
	UINT mCurUpperAnim;

	struct Animation
	{
		Animation(UINT animationType, UINT frameStart, UINT frameEnd, float animationSpeed)
		{
			this->AnimationType = animationType;
			this->FrameStart = frameStart;
			this->FrameEnd = frameEnd;
			this->playForwards = true;
			this->AnimationSpeed = animationSpeed;
		}

		Animation(UINT animationType, UINT frameStart, UINT frameEnd, bool playForwards, float animationSpeed)
		{
			this->AnimationType = animationType;
			this->FrameStart = frameStart;
			this->FrameEnd = frameEnd;
			this->playForwards = playForwards;
			this->AnimationSpeed = animationSpeed;
		}

		float AnimationSpeed;
		UINT FrameStart, FrameEnd;
		UINT AnimationType;
		bool playForwards;
	};

	std::vector<Animation> mAnimations;

	bool mFirstLowerAnimation;
	bool mFirstUpperAnimation;
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