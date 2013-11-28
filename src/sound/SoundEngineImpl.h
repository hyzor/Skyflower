#ifndef SOUND_SOUNDENGINEIMPL_H
#define SOUND_SOUNDENGINEIMPL_H

#include <vector>

#include <AL/al.h>
#include <AL/alc.h>

#include "Sound/Listener.h"
#include "Sound/SoundSource.h"
#include "ListenerImpl.h"
#include "SoundResource.h"
#include "TaskQueue.h"
#include "Util.h"
#include "Config.h"

class SoundEngineImpl
{
public:
	SoundEngineImpl();
	virtual ~SoundEngineImpl();

	bool Init();
	void Release();

	SoundSource *CreateSource();
	Listener *CreateListener();

	void DestroySource(SoundSource *source);
	void DestroyListener(Listener *listener);

	void PlaySound(const char *file, const float position[3], float volume, bool relativeToListener = false);

	void SetActiveListener(Listener *listener);
	void SetDopplerFactor(float dopplerFactor);
	void SetSpeedOfSound(float speed);

	void Update(float deltaTime);

private:
	// Allow SoundSources to get SoundResources.
	friend class SoundSourceImpl;

	// If the resource is not currently loaded, this function will block while reading the resource from disk.
	// These functions are thread safe.
	SoundResource *GetSoundResource(const std::string &name);
	void ReleaseSoundResource(SoundResource *resource);

	// Allow SoundResources to use the buffer pool.
	friend class SoundResourceWAV;

	// These functions are thread safe.
	ALuint GetBuffer();
	void ReleaseBuffer(ALuint buffer);

private:
	ALCdevice *m_device;
	ALCcontext *m_context;

	TaskQueue *m_taskQueue;

	ALuint m_bufferPool[SOUNDENGINE_BUFFER_POOL_SIZE];
	size_t m_bufferPoolSize;
	Mutex *m_bufferPoolMutex;

	Mutex *m_resourceMutex;

	ListenerImpl *m_activeListener;

	std::vector<RefCounted<SoundResource *>> m_soundResources;
	std::vector<SoundSource *> m_soundSources;

	std::vector<SoundSource *> m_sourcesPendingDeletion;
};

#endif
