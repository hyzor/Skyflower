
#ifndef INC_CISTRON
#define INC_CISTRON

#include "graphics/GraphicsEngine.h"
#include "InputHandler.h"
#include "Sound/SoundEngine.h"
#include "PotentialField.h"

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
	PotentialField* potentialField;
};

};

#endif
