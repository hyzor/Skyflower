#include "Sound/SoundEngine.h"
#include "SoundEngineImpl.h"

// Must be included last!
#include "debug.h"

DLL_EXPORT SoundEngine *CreateSoundEngine(const std::string &resourceDir)
{
	SoundEngineImpl *engine = new SoundEngineImpl();

	if (!engine->Init(resourceDir)) {
		delete engine;
		return NULL;
	}

	return (SoundEngine *)engine;
}

DLL_EXPORT void DestroySoundEngine(SoundEngine *engine)
{
	if (!engine) {
		return;
	}

	SoundEngineImpl *engineImpl = (SoundEngineImpl *)engine;
	engineImpl->Release();

	delete engineImpl;
}
