#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <algorithm>

#include <AL/al.h>
#include <AL/alc.h>

#include "SoundEngineImpl.h"
#include "SoundSourceImpl.h"
#include "AudioResource.h"
#include "ListenerImpl.h"
#include "TaskQueueWin32.h"

SoundEngineImpl::SoundEngineImpl()
{
	m_device = NULL;
	m_context = NULL;

	m_resourceCache = NULL;
	m_activeListener = NULL;
}

SoundEngineImpl::~SoundEngineImpl()
{
}

bool SoundEngineImpl::Init(const std::string &resourceDir)
{
	m_device = alcOpenDevice(NULL);

	if (!m_device) {
		printf("Failed to open OpenAL device\n");
		return false;
	}

	m_context = alcCreateContext(m_device, NULL);

	if (!m_context) {
		printf("Failed to create OpenAL context\n");
		alcCloseDevice(m_device);
		return false;
	}

	alcMakeContextCurrent(m_context);

	printf("OpenAL '%s' running on '%s'\n", alGetString(AL_VERSION), alGetString(AL_RENDERER));

#if 1
	ALCenum error = alcGetError(m_device);

	if (error != ALC_NO_ERROR)
		printf("alcGetError=%s\n", alcGetString(m_device, error));
#endif

	m_resourceCache = new ResourceCache(resourceDir);
	m_activeListener = NULL;

	return true;
}

void SoundEngineImpl::Release()
{
#if 1
	ALCenum error = alcGetError(m_device);

	if (error != ALC_NO_ERROR)
		printf("alcGetError=%s\n", alcGetString(m_device, error));
#endif

	SoundSource *source;

	while (!m_sources.empty()){
		source = m_sources.back();
		m_sources.pop_back();

		delete source;
	}

	delete m_resourceCache;

	alcMakeContextCurrent(NULL);
	alcDestroyContext(m_context);
	alcCloseDevice(m_device);
}

SoundSource *SoundEngineImpl::CreateSource()
{
	SoundSource *source = (SoundSource *)new SoundSourceImpl(m_resourceCache);
	m_sources.push_back(source);

	return source;
}

Listener *SoundEngineImpl::CreateListener()
{
	Listener *listener = (Listener *)new ListenerImpl();

	return listener;
}

void SoundEngineImpl::DestroySource(SoundSource *source)
{
	if (!source) {
		return;
	}

	for (size_t i = 0; i < m_sources.size(); i++) {
		if (m_sources[i] == source) {
			m_sources[i] = m_sources.back();
			m_sources.pop_back();

			delete source;

			break;
		}
	}
}

void SoundEngineImpl::DestroyListener(Listener *listener)
{
	if (!listener) {
		return;
	}

	ListenerImpl *listenerImpl = (ListenerImpl *)listener;

	if (listenerImpl == m_activeListener) {
		m_activeListener = NULL;
	}

	delete listenerImpl;
}

void SoundEngineImpl::SetActiveListener(Listener *listener)
{
	m_activeListener = (ListenerImpl *)listener;
}

void SoundEngineImpl::SetDopplerFactor(float dopplerFactor)
{
	alDopplerFactor(dopplerFactor);
}

void SoundEngineImpl::SetSpeedOfSound(float speed)
{
	alSpeedOfSound(speed);
}

void SoundEngineImpl::PlaySound(const char *file, const float position[3], float volume, bool relativeToListener)
{
	assert(0);
}

void SoundEngineImpl::Update(float deltaTime)
{
	ListenerImpl *activeListener = (ListenerImpl *)m_activeListener;
	SoundSourceImpl *source;

	if (activeListener) {
		alListenerf(AL_GAIN, activeListener->m_volume);
		alListenerfv(AL_POSITION, activeListener->m_position);
		alListenerfv(AL_ORIENTATION, activeListener->m_orientation);
		alListenerfv(AL_VELOCITY, activeListener->m_velocity);
	}

	m_resourceCache->Update();

	for (auto i = m_sources.begin(); i != m_sources.end(); i++) {
		source = (SoundSourceImpl *)(*i);
		source->Update();
	}

#if 1
	ALenum error = alGetError();

	if (error != AL_NO_ERROR)
		printf("alGetError=%s (0x%x)\n", alGetString(error), error);
#endif
}
