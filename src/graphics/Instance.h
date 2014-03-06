#ifndef INSTANCE_H
#define INSTANCE_H

#include "shared/platform.h"
#include "shared/Vec3.h"

class DLL_API ModelInstance
{
public:
	virtual ~ModelInstance() {};

	virtual bool IsVisible() = 0;
	virtual Vec3 GetPosition() = 0;
	virtual Vec3 GetRotation() = 0;
	virtual Vec3 GetScale() = 0;

	virtual void SetPosition(Vec3 pos) = 0;
	virtual void SetRotation(Vec3 rot) = 0;
	virtual void SetScale(Vec3 scale) = 0;
	virtual void SetVisibility(bool visible) = 0;
	virtual void SetLightFrustumCalcFlag(bool flag) = 0;
	virtual void Set(Vec3 pos, Vec3 rot, Vec3 scale) = 0;
	virtual bool GetLightFrustumCalcFlag() const = 0;
	virtual int GetType() = 0;
	virtual void SetType(int type) = 0;

};

class DLL_API AnimatedInstance
{
public:
	virtual ~AnimatedInstance() {};

	virtual bool IsVisible() = 0;
	virtual Vec3 GetPosition() = 0;
	virtual Vec3 GetRotation() = 0;
	virtual Vec3 GetScale() = 0;
	virtual UINT GetAnimation() = 0;
	virtual bool IsAnimationDone() = 0;

	virtual void SetPosition(Vec3 pos) = 0;
	virtual void SetRotation(Vec3 rot) = 0;
	virtual void SetScale(Vec3 scale) = 0;
	virtual void SetVisibility(bool visible) = 0;
	virtual void Set(Vec3 pos, Vec3 rot, Vec3 scale) = 0;

	virtual void CreateAnimation(int id, int start, int frames) = 0;
	virtual void CreateAnimation(int id, int start, int frames, bool playForwards) = 0;
	virtual void SetAnimation(UINT id, bool loop) = 0;
	virtual void SetAnimationSpeed(UINT id, float speed) = 0;

};


class DLL_API SortedAnimatedInstance
{
public:
	virtual ~SortedAnimatedInstance() {};

	virtual bool IsVisible() = 0;
	virtual Vec3 GetPosition() = 0;
	virtual Vec3 GetRotation() = 0;
	virtual Vec3 GetScale() = 0;
	virtual UINT GetLowerAnimation() = 0;
	virtual bool IsLowerAnimationDone() = 0;
	virtual UINT GetUpperAnimation() = 0;
	virtual bool IsUpperAnimationDone() = 0;

	virtual void SetPosition(Vec3 pos) = 0;
	virtual void SetRotation(Vec3 rot) = 0;
	virtual void SetScale(Vec3 scale) = 0;
	virtual void SetVisibility(bool visible) = 0;
	virtual void Set(Vec3 pos, Vec3 rot, Vec3 scale) = 0;

	virtual void CreateAnimation(int id, int start, int frames) = 0;
	virtual void CreateAnimation(int id, int start, int frames, bool playForwards) = 0;

	virtual void SetLowerAnimation(UINT id, bool loop) = 0;
	virtual void SetLowerAnimationSpeed(UINT id, float speed) = 0;
	virtual void SetUpperAnimation(UINT id, bool loop) = 0;
	virtual void SetUpperAnimationSpeed(UINT id, float speed) = 0;
};

class DLL_API MorphModelInstance
{
public:
	virtual ~MorphModelInstance() {};

	virtual bool IsVisible() const = 0;
	virtual Vec3 GetPosition() const = 0;
	virtual Vec3 GetRotation() const = 0;
	virtual Vec3 GetScale() const = 0;
	virtual Vec3 GetWeights() const = 0;

	virtual void Set(Vec3 pos, Vec3 rot, Vec3 scale, Vec3 weights) = 0;
	virtual void SetVisibility(bool visible) = 0;
	virtual void SetPosition(Vec3 pos) = 0;
	virtual void SetRotation(Vec3 rot) = 0;
	virtual void SetScale(Vec3 scale) = 0;
	virtual void SetWeights(Vec3 weights) = 0;
	
};
#endif
