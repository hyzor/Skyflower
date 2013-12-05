#include "shared/platform.h"

#include "Sound/SoundEngine.h"
#include "SoundEngineImpl.h"

DLL_API SoundEngine *CreateSoundEngine(const std::string &resourceDir)
{
	SoundEngineImpl *engine = new SoundEngineImpl();

	if (!engine->Init(resourceDir)) {
		delete engine;
		return NULL;
	}

	return (SoundEngine *)engine;
}

DLL_API void DestroySoundEngine(SoundEngine *engine)
{
	if (!engine) {
		return;
	}

	SoundEngineImpl *engineImpl = (SoundEngineImpl *)engine;
	engineImpl->Release();

	delete engineImpl;
}
