#include "shared/platform.h"

#include "Sound/SoundEngine.h"
#include "SoundEngineImpl.h"

DLL_API SoundEngine *CreateSoundEngine()
{
	SoundEngine *engine = (SoundEngine *)new SoundEngineImpl();

	return engine;
}

DLL_API void DestroySoundEngine(SoundEngine *engine)
{
	delete engine;
}
