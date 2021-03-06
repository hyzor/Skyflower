#ifndef EVENT_H
#define EVENT_H

#include "Cistron.h"
#include <string>
#include <iostream>
#include <sstream>
#include "shared/Vec3.h"
#include "Entity.h"
#include "ScriptHandler.h"
#include "Components/Push.h"
#include "LevelHandler.h"

using namespace std;
using namespace Cistron;

class Event : public Component {

public:

	Event(std::string file) : Component("Event")
	{
		this->file = file;
		startUpdate = false;
		firstUpdate = true;
		activated = false;

	};
	virtual ~Event() {};

	static void Register(ScriptHandler* sh)
	{
		lua_register(sh->L, "PlaySound", Event::PlaySound);
		lua_register(sh->L, "Jump", Event::Jump);
		lua_register(sh->L, "ChangeLevel", Event::ChangeLevel);
		lua_register(sh->L, "Level", Event::Level);
		lua_register(sh->L, "Save", Event::Save);
		lua_register(sh->L, "Load", Event::Load);
		lua_register(sh->L, "Spawn", Event::Spawn);
		lua_register(sh->L, "StopUpdate", Event::StopUpdate);
		lua_register(sh->L, "StartUpdate", Event::StartUpdate);
		lua_register(sh->L, "InRange", Event::InRange);
		lua_register(sh->L, "IsActivated", Event::IsActivated);
		lua_register(sh->L, "IsActivator", Event::IsActivator);
		lua_register(sh->L, "IsDown", Event::IsDown);
		lua_register(sh->L, "PrintText", Event::PrintText);
		lua_register(sh->L, "PlayFinishedSound", Event::PlayFinishedSound);

		//AI
		lua_register(sh->L, "SetTarget", Event::SetTarget);
		lua_register(sh->L, "IsTouching", Event::IsTouching);
		lua_register(sh->L, "IsStanding", Event::IsStanding);
		lua_register(sh->L, "CanPush", Event::CanPush);
		lua_register(sh->L, "Push", Event::push);
		lua_register(sh->L, "PushAll", Event::pushAll);
		lua_register(sh->L, "SetSpeed", Event::SetSpeed);
		lua_register(sh->L, "IsDizzy", Event::IsDizzy);
		//Throw
		lua_register(sh->L, "PickUp", Event::PickUp);
		lua_register(sh->L, "PickUpAll", Event::PickUpAll);
		lua_register(sh->L, "CanPick", Event::CanPick);
		lua_register(sh->L, "Throw", Event::sThrow);
		lua_register(sh->L, "CanThrow", Event::CanThrow);
		lua_register(sh->L, "IsHeld", Event::IsHeld);

		// MoveTarget component
		lua_register(sh->L, "MoveToTarget", Event::MoveToTarget);
		lua_register(sh->L, "MoveToSpawn", Event::MoveToSpawn);
		lua_register(sh->L, "SetContinous", Event::SetContinous);

		// Lights
		lua_register(sh->L, "Lit", Event::Lit);
		lua_register(sh->L, "Unlit", Event::Unlit);
		lua_register(sh->L, "IsLit", Event::IsLit);
		lua_register(sh->L, "IsUnlit", Event::IsUnlit);

		// Balloon
		lua_register(sh->L, "Pop", Event::Pop);

		// Morph
		lua_register(sh->L, "SetMorphState", Event::SetMorphState);
		lua_register(sh->L, "StartMorph", Event::StartMorph);
		lua_register(sh->L, "CompletedLevelCount", Event::CompletedLevelCount);

		//to deactivate the button
		lua_register(sh->L, "ButtonUp", Event::ButtonUp);

		//for platform-puzzles
		lua_register(sh->L, "FallingPlatform", Event::FallingPlatform);

		//for box-puzzle
		lua_register(sh->L, "OnButton", Event::OnButton);
		lua_register(sh->L, "IsAlive", Event::IsAlive);
		lua_register(sh->L, "Respawn", Event::Respawn);
		lua_register(sh->L, "SetCanMoveBox", Event::SetCanMoveBox);
		lua_register(sh->L, "SetCanNotMoveBox", Event::SetCanNotMoveBox);

		//Determine which sign to use in the hubworld, with or without a mark that indicates that you have completed a subworld
		lua_register(sh->L, "LevelIsCompleted", Event::LevelIsCompleted);
		lua_register(sh->L, "SetActivated", Event::SetActivated);





	};

	void addedToEntity();
	void update(float deltaTime);

	static EntityManager* entityManager;
	static Event* self;

	bool isActivated()
	{
		return activated;
	}

private:

	std::string file;

	bool startUpdate;
	bool firstUpdate;
	bool activated;

	void Activated(Message const& msg);
	void Deactivated(Message const& msg);

	static int PlaySound(lua_State* L);
	static int Jump(lua_State* L);
	static int ChangeLevel(lua_State* L);
	static int Level(lua_State* L);
	static int Save(lua_State* L);
	static int Load(lua_State* L);
	static int Spawn(lua_State* L);
	static int StartUpdate(lua_State* L);
	static int StopUpdate(lua_State* L);
	static int InRange(lua_State* L);
	static int IsActivated(lua_State* L);
	static int IsActivator(lua_State* L);
	static int IsDown(lua_State* L);
	static int PlayFinishedSound(lua_State* L);

	static int SetTarget(lua_State* L);
	static int IsTouching(lua_State* L);
	static int IsStanding(lua_State* L);
	static int CanPush(lua_State* L);
	static int push(lua_State* L);
	static int pushAll(lua_State* L);
	static int SetSpeed(lua_State* L);
	static int IsDizzy(lua_State* L);

	static int PickUp(lua_State* L);
	static int PickUpAll(lua_State* L);
	static int CanPick(lua_State* L);
	static int sThrow(lua_State* L);
	static int CanThrow(lua_State* L);
	static int IsHeld(lua_State* L);

	static int MoveToTarget(lua_State* L);
	static int MoveToSpawn(lua_State* L);
	static int SetContinous(lua_State* L);

	static int Lit(lua_State* L);
	static int Unlit(lua_State* L);
	static int IsLit(lua_State* L);
	static int IsUnlit(lua_State* L);

	static int Pop(lua_State* L);
	static int PrintText(lua_State* L);

	static int SetMorphState(lua_State* L);
	static int StartMorph(lua_State* L);
	static int CompletedLevelCount(lua_State* L);

	static int ButtonUp(lua_State* L);

	static int FallingPlatform(lua_State* L);

	static int LevelIsCompleted(lua_State* L);
	static int SetActivated(lua_State* L);

	static int OnButton(lua_State* L);
	static int IsAlive(lua_State* L);
	static int Respawn(lua_State* L);
	static int SetCanMoveBox(lua_State* L);
	static int SetCanNotMoveBox(lua_State* L);
};

#endif