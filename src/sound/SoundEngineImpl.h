#ifndef SOUND_SOUNDENGINEIMPL_H
#define SOUND_SOUNDENGINEIMPL_H

#include <vector>

#include <AL/al.h>
#include <AL/alc.h>

#include "Sound/Listener.h"
#include "Sound/SoundSource.h"
#include "Config.h"
#include "ListenerImpl.h"
#include "ResourceCache.h"

class SoundEngineImpl
{
public:
	SoundEngineImpl();
	virtual ~SoundEngineImpl();

	bool Init(const std::string &resourceDir);
	void Release();

	virtual SoundSource *CreateSource();
	virtual Listener *CreateListener();

	virtual void DestroySource(SoundSource *source);
	virtual void DestroyListener(Listener *listener);

	virtual void PlaySound(const char *file, const float position[3], float volume, bool relativeToListener = false);

	virtual void SetActiveListener(Listener *listener);
	virtual void SetDopplerFactor(float dopplerFactor);
	virtual void SetSpeedOfSound(float speed);

	virtual void Update(float deltaTime);

private:
	ALCdevice *m_device;
	ALCcontext *m_context;

	ResourceCache *m_resourceCache;
	ListenerImpl *m_activeListener;

	std::vector<SoundSource *> m_sources;
};

#endif
