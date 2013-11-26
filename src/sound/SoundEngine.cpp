#include "shared/platform.h"

#include "Sound/SoundEngine.h"
#include "SoundEngineImpl.h"

DLL_API SoundEngine *CreateSoundEngine()
{
	SoundEngineImpl *engine = new SoundEngineImpl();

	if (!engine->Init())
	{
		delete engine;
		return NULL;
	}

	return (SoundEngine *)engine;
}

DLL_API void DestroySoundEngine(SoundEngine *engine)
{
	SoundEngineImpl *engineImpl = (SoundEngineImpl *)engine;
	engineImpl->Release();

	delete engineImpl;
}