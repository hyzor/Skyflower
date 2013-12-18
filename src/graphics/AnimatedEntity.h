#ifndef ANIMATEDENTITY_H_
#define ANIMATEDENTITY_H_

#include "GenericSkinnedModel.h"
#include "Camera.h"
#include "ShaderHandler.h"

class AnimatedEntity
{
public:
	AnimatedEntity(void);
	AnimatedEntity(GenericSkinnedModel* model, XMFLOAT3 position);
	~AnimatedEntity(void);

	//void Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, Camera* mCamera);
	void Draw(ID3D11DeviceContext* dc, Camera* cam, NormalMappedSkinned* shader, XMMATRIX &world);

	void Update(float dt);

	void SetPosition(XMFLOAT3 pos);
	void RotateXYZ(XMFLOAT3 rot, float yaw, XMVECTOR Up);

	void SetKeyFrameInterval(UINT frameStart, UINT frameEnd) { mInstance.frameStart = frameStart; mInstance.frameEnd = frameEnd; }
	void PlayAnimationForwards() { mInstance.playAnimForward = true; }
	void PlayAnimationBackwards() { mInstance.playAnimForward = false; }

	XMFLOAT3 Position;
	XMFLOAT3 Scale;
	float Rotation;

	//-------------------------------------------
	// Animations
	//-------------------------------------------
	enum AnimationTypes
	{
		IdleAnim = 0,
		RunningAnim,
		RunningBackwardsAnim,
		JumpingAnim,
		StrafingRightAnim,
		StrafingLeftAnim
	};

	struct Animation
	{
		Animation(UINT animationType, UINT frameStart, UINT frameEnd)
		{
			this->AnimationType = animationType;
			this->FrameStart = frameStart;
			this->FrameEnd = frameEnd;
		}

		UINT FrameStart, FrameEnd;
		UINT AnimationType;
	};

	std::vector<Animation> mAnimations;
	UINT mCurAnim;

	GenericSkinnedModelInstance mInstance;
};

#endif