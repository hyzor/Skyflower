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
	virtual void Set(Vec3 pos, Vec3 rot, Vec3 scale) = 0;
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

	virtual void SetPosition(Vec3 pos) = 0;
	virtual void SetRotation(Vec3 rot) = 0;
	virtual void SetScale(Vec3 scale) = 0;
	virtual void SetVisibility(bool visible) = 0;
	virtual void Set(Vec3 pos, Vec3 rot, Vec3 scale) = 0;

	virtual void CreateAnimation(int id, int start, int frames) = 0;
	virtual void CreateAnimation(int id, int start, int frames, bool playForwards) = 0;
	virtual void SetAnimation(UINT id, bool loop) = 0;

};

#endif
