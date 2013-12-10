#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cstdio>

#include <AL/al.h>
#include <AL/alc.h>

#include "AudioResource.h"
#include "ListenerImpl.h"
#include "SoundEngineImpl.h"
#include "SoundSourceImpl.h"
#include "TaskQueueWin32.h"

static float vec3_distance_squared(const float vec1[3], const float vec2[3])
{
	float x = vec2[0] - vec1[0];
	float y = vec2[1] - vec1[1];
	float z = vec2[2] - vec1[2];

	return x * x + y * y + z * z;
}

static std::vector<SoundSource *> ApplyCullingDistance(const Listener *listener, const std::vector<SoundSource *> &sources)
{
	std::vector<SoundSource *> result;
	float listenerPosition[3], sourcePosition[3];
	SoundSourceImpl *source;

	result.reserve(sources.size());
	listener->GetPosition(listenerPosition);

	for (auto iter = sources.begin(); iter != sources.end(); iter++) {
		source = (SoundSourceImpl *)(*iter);
		source->GetPosition(sourcePosition);

		if (vec3_distance_squared(listenerPosition, sourcePosition) <= (SOUNDENGINE_CULLING_DISTANCE * SOUNDENGINE_CULLING_DISTANCE)) {
			result.push_back(source);
		}
	}

	if (sources.size() - result.size() > 0) {
		//printf("Culled %u sound sources\n", sources.size() - result.size());
	}

	return result;
}

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

	if (error != ALC_NO_ERROR) {
		printf("alcGetError=%s\n", alcGetString(m_device, error));
	}
#endif

	alGenSources(SOUNDENGINE_SOURCE_POOL_SIZE, m_sourcePool);

	for (int i = 0; i < SOUNDENGINE_SOURCE_POOL_SIZE; i++) {
		m_availableSources.push(m_sourcePool[i]);
	}

	m_resourceCache = new ResourceCache(resourceDir);
	m_activeListener = NULL;

	// Make sure we cull directly on the first update. (Don't really need it here, instead set it when creating a source)
	m_timeAccumulationCulling = FLT_MAX;

	return true;
}

void SoundEngineImpl::Release()
{
#if 1
	ALCenum error = alcGetError(m_device);

	if (error != ALC_NO_ERROR) {
		printf("alcGetError=%s\n", alcGetString(m_device, error));
	}
#endif

	SoundSourceImpl *source;

	while (!m_sources.empty()) {
		source = (SoundSourceImpl *)m_sources.back();
		m_sources.pop_back();

		if (source->HasSource()) {
			m_availableSources.push(source->RevokeSource());
		}

		delete source;
	}

	delete m_resourceCache;

	alDeleteSources(SOUNDENGINE_SOURCE_POOL_SIZE, m_sourcePool);

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

	SoundSourceImpl *sourceImpl = (SoundSourceImpl *)source;

	for (size_t i = 0; i < m_sources.size(); i++) {
		if (m_sources[i] == sourceImpl) {
			m_sources[i] = m_sources.back();
			m_sources.pop_back();

			if (sourceImpl->HasSource()) {
				m_availableSources.push(sourceImpl->RevokeSource());
			}

			delete sourceImpl;

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
	// FIXME: Implement!
	assert(0);
}

void SoundEngineImpl::Update(float deltaTime)
{
	ListenerImpl *activeListener = (ListenerImpl *)m_activeListener;
	SoundSourceImpl *source;

	if (!activeListener) {
		return;
	}

	activeListener->ApplyState();
	m_resourceCache->Update();

	m_timeAccumulationCulling += deltaTime;

	// FIXME: Directly add newly created sources to m_activeSources if they are within the culling distance.
	// Or set m_timeAccumulationCulling to FLT_MAX when adding new sources?

	if (m_timeAccumulationCulling >= (1.0f / SOUNDENGINE_CULLING_FREQUENCY)) {
		m_timeAccumulationCulling = 0.0f;

		std::vector<SoundSource *> previousActiveSources = m_activeSources;
		m_activeSources = ApplyCullingDistance(m_activeListener, m_sources);

		std::vector<SoundSource *> difference;
		std::set_difference(previousActiveSources.begin(), previousActiveSources.end(), m_activeSources.begin(), m_activeSources.end(), std::inserter(difference, difference.begin()));

		// difference contains newly deactivated sources.
		for (auto iter = difference.begin(); iter != difference.end(); iter++) {
			source = (SoundSourceImpl *)(*iter);
			m_availableSources.push(source->RevokeSource());
		}

		difference.clear();
		std::set_difference(m_activeSources.begin(), m_activeSources.end(), previousActiveSources.begin(), previousActiveSources.end(), std::inserter(difference, difference.begin()));

		// difference contains newly activated sources.
		for (auto iter = difference.begin(); iter != difference.end(); iter++) {
			if (m_availableSources.empty()) {
				printf("We are out of sources to lend!\n");
				break;
			}

			source = (SoundSourceImpl *)(*iter);
			source->LendSource(m_availableSources.top());
			m_availableSources.pop();
		}
	}

	for (auto iter = m_sources.begin(); iter != m_sources.end(); iter++) {
		source = (SoundSourceImpl *)(*iter);
		source->Update(deltaTime);
	}

#if 1
	ALenum error = alGetError();

	if (error != AL_NO_ERROR) {
		printf("alGetError=%s (0x%x)\n", alGetString(error), error);
	}
#endif
}
