#ifndef SKYFLOWER_COMPONENTS_PARTICLESYSTEMCOMP_H
#define SKYFLOWER_COMPONENTS_PARTICLESYSTEMCOMP_H

#include "shared/Vec3.h"
#include "graphics/instance.h"
#include "Cistron.h"
#include "Entity.h"
#include "Component.h"
#include "ScriptHandler.h"

#include <string>

using namespace std;
using namespace Cistron;

class ParticleSystemComp : public Component
{
private:

	//Pointer to the actual object that will be rendered by the graphics engine
	ParticleSystem* mParticleSystem;

	//ID used to manage a entity with several particle systems
	UINT mParticleSystemID;

	//Flag used to control the activity of the actual particle system contained within the component
	bool mParticleSystemIsActive;

	//Used to call the correct Luascript corresponding to this specific particle system
	string mScriptName;

	static int SetEmitFrequency(lua_State* L);
	static int SetAgeLimit(lua_State* L);
	static int SetFadeTime(lua_State* L);
	static int SetEmitPos(lua_State* L);
	static int SetAcceleration(lua_State* L);
	static int SetDirection(lua_State* L);
	static int SetRandomVelocity(lua_State* L);
	static int SetRandomVelocityActive(lua_State* L);
	static int SetParticleType(lua_State* L);
	static int SetScale(lua_State* L);

	static int GetEmitFrequency(lua_State* L);
	static int GetAgeLimit(lua_State* L);
	static int GetFadeTime(lua_State* L);
	static int GetEmitPos(lua_State* L);
	static int GetAcceleration(lua_State* L);
	static int GetDirection(lua_State* L);
	static int GetRandomVelocity(lua_State* L);
	static int GetRandomVelocityActive(lua_State* L);

	static int Activate(lua_State* L);
	static int Deactivate(lua_State* L);

	//Functions called when requested messages are received
	void ActivateParticleSystem(const Message& message);
	void DeactivateParticleSystem(const Message& message);

	void _SetParticleType();

	//Variables containing data that define the behaviour of the particle system
	//Vec3 mPosition;
	//Vec3 mAcceleration;
	//Vec3 mDirection;
	//Vec3 mRandomVelocity;
	//float mAgeLimit;
	//float mFadeTime;
	//float mEmitFrequency;

	//void pSetEmitFrequency(float frequency);
	//void pSetAgeLimit(float ageLimit);
	//void pSetFadeTime(float fadeTime);
	//void pSetEmitPos(Vec3 pos);
	//void pSetAcceleration(Vec3 acceleration);
	//void pSetDirection(Vec3 direction);
	//void pSetRandomVelocity(Vec3 velocity);
	//void pSetRandomVelocityActive(bool flag);

	//float pGetEmitFrequency() const;
	//float pGetAgeLimit() const;
	//float pGetFadeTime() const;
	//Vec3 pGetEmitPos() const;
	//Vec3 pGetAcceleration() const;
	//Vec3 pGetDirection() const;
	//Vec3 pGetRandomVelocity() const;
	//bool pGetRandomVelocityActive() const;

	////Activate or deactivate the particle system
	//void pActivate();
	//void pDeactivate();
public:

	ParticleSystemComp(string scriptName, UINT ID);
	virtual ~ParticleSystemComp(){}

	void addedToEntity();
	void removeFromEntity();
	void update(float deltaTime);

	ParticleSystem* GetParticleSystem() { return this->mParticleSystem; }
	bool ParticleSystemIsActive() const { return this->mParticleSystemIsActive; }

	void SetParticleSystemIsActive(bool flag) { this->mParticleSystemIsActive = flag; }

	static void Register(ScriptHandler* sh)
	{
		lua_register(sh->L, "SetEmitFrequency", ParticleSystemComp::SetEmitFrequency);
		lua_register(sh->L, "SetAgeLimit", ParticleSystemComp::SetAgeLimit);
		lua_register(sh->L, "SetFadeTime", ParticleSystemComp::SetFadeTime);
		lua_register(sh->L, "SetEmitPos", ParticleSystemComp::SetEmitPos);
		lua_register(sh->L, "SetAcceleration", ParticleSystemComp::SetAcceleration);
		lua_register(sh->L, "SetDirection", ParticleSystemComp::SetDirection);
		lua_register(sh->L, "SetRandomVelocity", ParticleSystemComp::SetRandomVelocity);
		lua_register(sh->L, "SetRandomVelocityActive", ParticleSystemComp::SetRandomVelocityActive);
		lua_register(sh->L, "SetParticleType", ParticleSystemComp::SetParticleType);
		lua_register(sh->L, "SetScale", ParticleSystemComp::SetScale);

		//lua_register(sh->L, "GetEmitFrequency", ParticleSystemComp::GetEmitFrequency);
		//lua_register(sh->L, "GetAgeLimit", ParticleSystemComp::GetAgeLimit);
		//lua_register(sh->L, "GetFadeTime", ParticleSystemComp::GetFadeTime);
		//lua_register(sh->L, "GetEmitPos", ParticleSystemComp::GetEmitPos);
		//lua_register(sh->L, "GetAcceleration", ParticleSystemComp::GetAcceleration);
		//lua_register(sh->L, "GetDirection", ParticleSystemComp::GetDirection);
		//lua_register(sh->L, "GetRandomVelocity", ParticleSystemComp::GetRandomVelocity);
		//lua_register(sh->L, "GetRandomVelocityActive", ParticleSystemComp::GetRandomVelocityActive);

		lua_register(sh->L, "Activate", ParticleSystemComp::Activate);
		lua_register(sh->L, "Deactivate", ParticleSystemComp::Deactivate);
	};

	public:
		static EntityManager* mEntityManager;
};

#endif
