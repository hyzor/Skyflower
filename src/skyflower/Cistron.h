
#ifndef INC_CISTRON
#define INC_CISTRON

//#include "GUI.h"
#include "graphics/GraphicsEngine.h"
#include "InputHandler.h"
#include "Sound/SoundEngine.h"
#include "physics/PhysicsEngine.h"
#include "physics/Collision.h"
#include "PotentialField.h"
#include "ScriptHandler.h"
#include "graphics/CameraController.h"


// IMPORTANT!
// decide below which version you want - a version using boost::any or a version using a simple void pointer

// boost version
//#include <boost/any.hpp>
//#define Payload boost::any

// boostless version - uses plain void pointer
#define Payload void*

namespace Cistron {

// Entity & component id
typedef int EntityId;
typedef int ComponentId;

struct Modules
{
	InputHandler *input;
	GraphicsEngine *graphics;
	SoundEngine *sound;
	CameraController* camera;
	PhysicsEngine* physicsEngine;
	Collision *collision;
	PotentialField* potentialField;
	ScriptHandler* script;
	//GUI* gui;
};

};

#endif
