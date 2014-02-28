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

	float mParticleSystemFadeTime;
	float mParticleSystemCurrFadeTime;

	//Set-functions called by Lua
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
	static int SetFadeLimit(lua_State* L);
	static int SetColor(lua_State* L);
	static int Activate(lua_State* L);
	static int Deactivate(lua_State* L);

	//Functions called when requested messages are received
	void ActivateParticleSystem(const Message& message);
	void DeactivateParticleSystem(const Message& message);

	void _SetParticleType();
public:

	ParticleSystemComp(string scriptName, UINT ID);
	virtual ~ParticleSystemComp(){}

	//General component functions
	void addedToEntity();
	void removeFromEntity();
	void update(float deltaTime);

	//Get-functions
	ParticleSystem* GetParticleSystem() { return this->mParticleSystem; }
	bool ParticleSystemIsActive() const { return this->mParticleSystemIsActive; }

	//Set-functions
	void SetParticleSystemIsActive(bool flag) { this->mParticleSystemIsActive = flag; }
	void SetParticleSystemFadeLimit(float limit) { this->mParticleSystemFadeTime = limit; }

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
		lua_register(sh->L, "SetFadeLimit", ParticleSystemComp::SetFadeLimit);
		lua_register(sh->L, "SetColor", ParticleSystemComp::SetColor);
		lua_register(sh->L, "Activate", ParticleSystemComp::Activate);
		lua_register(sh->L, "Deactivate", ParticleSystemComp::Deactivate);
	};

	public:
		static EntityManager* mEntityManager;
};

#endif
