#ifndef CUTSCENE_H
#define CUTSCENE_H

#include "ScriptHandler.h"
#include "EntityManager.h"
#include "shared\Vec3.h"
#include "graphics\CameraController.h"

class CutScene
{
public:
	struct WayPoint
	{
		Vec3 _position;
		float _yaw;
		float _pitch;
		float _time;
		WayPoint() { _position = Vec3(); _yaw = 0.0f; _pitch = 0.0f; _time = 0.0f; }
		
		WayPoint(Vec3 position, float yaw, float pitch, float time)
			: _position(position), _yaw(yaw), _pitch(pitch), _time(time) {}
	};

	CutScene(ScriptHandler* sh, CameraController* camera);
	void play();

	static void Register(ScriptHandler* sh)
	{
		lua_register(sh->L, "AddPoint", CutScene::AddPoint);
		lua_register(sh->L, "SetCamera", CutScene::SetCamera);
	};

	void update(float dt);
	bool isPlaying() const { return !done; }
	void stop();
public:
	static CutScene* self;

private:
	vector<WayPoint> mWaypoints;
	ScriptHandler* mScriptHandlerPtr;
	CameraController* mCameraPtr;

	int mCurrentWP;
	float mCurrentYaw;
	float mCurrentPitch;
	bool done;
private:
	static int AddPoint(lua_State* L);
	static int SetCamera(lua_State* L);

};



#endif