#ifndef SOUND_SOUNDENGINEIMPL_H
#define SOUND_SOUNDENGINEIMPL_H

#include <stack>
#include <vector>

#include <AL/al.h>
#include <AL/alc.h>

#include "Sound/Listener.h"
#include "Sound/SoundEngine.h"
#include "Sound/SoundSource.h"
#include "Config.h"
#include "ListenerImpl.h"
#include "SoundSourceImpl.h"
#include "ResourceCache.h"

class SoundEngineImpl : public SoundEngine
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

	virtual void PlaySound(const std::string &file, const float position[3], float volume, bool relativeToListener);

	virtual void SetActiveListener(Listener *listener);
	virtual void SetDopplerFactor(float dopplerFactor);
	virtual void SetSpeedOfSound(float speed);

	virtual void Update(float deltaTime);

private:
	ALCdevice *m_device;
	ALCcontext *m_context;

	ALuint m_sourcePool[SOUNDENGINE_SOURCE_POOL_SIZE];
	std::stack<ALuint> m_availableSources;

	ResourceCache *m_resourceCache;
	ListenerImpl *m_activeListener;

	float m_timeAccumulationCulling;

	std::vector<SoundSourceImpl *> m_sources;
	// Sources created with PlaySound (They are immediately removed when they have finished playing)
	std::vector<SoundSourceImpl *> m_temporarySources;
	std::vector<SoundSourceImpl *> m_activeSources;
};

#endif
