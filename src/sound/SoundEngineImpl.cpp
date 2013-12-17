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

static float vec3_length_squared(const float vec[3])
{
	return vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2];
}

static float vec3_distance_squared(const float vec1[3], const float vec2[3])
{
	float difference[3] = {
		vec2[0] - vec1[0],
		vec2[1] - vec1[1],
		vec2[2] - vec1[2]
	};

	return vec3_length_squared(difference);
}

static std::vector<SoundSourceImpl *> ApplyCullingDistance(const Listener *listener, const std::vector<SoundSourceImpl *> &sources)
{
	std::vector<SoundSourceImpl *> result;
	float listenerPosition[3], sourcePosition[3];
	float distanceSquared;
	SoundSourceImpl *source;

	result.reserve(sources.size());
	listener->GetPosition(listenerPosition);

	for (auto iter = sources.begin(); iter != sources.end(); iter++) {
		source = (*iter);
		source->GetPosition(sourcePosition);

		if (source->IsRelativeToListener()) {
			distanceSquared = vec3_length_squared(sourcePosition);
		}
		else {
			distanceSquared = vec3_distance_squared(listenerPosition, sourcePosition);
		}

		if (distanceSquared <= (SOUNDENGINE_CULLING_DISTANCE * SOUNDENGINE_CULLING_DISTANCE)) {
			result.push_back(source);
		}
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
	//printf("Available OpenAL extensions: %s\n", alGetString(AL_EXTENSIONS));

	// AL_EXT_float32 is required by the opus decoder.
	assert(alIsExtensionPresent("AL_EXT_float32"));

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

	m_timeAccumulationCulling = 0.0f;

	return true;
}

void SoundEngineImpl::Release()
{
	SoundSourceImpl *source;

	while (!m_sources.empty()) {
		source = m_sources.back();
		m_sources.pop_back();

		if (source->HasSource()) {
			m_availableSources.push(source->RevokeSource());
		}

		delete source;
	}

	while (!m_temporarySources.empty()) {
		source = m_temporarySources.back();
		m_temporarySources.pop_back();

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
	SoundSourceImpl *source = new SoundSourceImpl(m_resourceCache);
	m_sources.push_back(source);

	// Perform a cull on the next update.
	m_timeAccumulationCulling = FLT_MAX;

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

	for (auto iter = m_activeSources.begin(); iter != m_activeSources.end(); iter++) {
		if ((*iter) == sourceImpl) {
			m_activeSources.erase(iter);
			break;
		}
	}

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

void SoundEngineImpl::PlaySound(const std::string &file, const float position[3], float volume, bool relativeToListener)
{
	SoundSourceImpl *source = new SoundSourceImpl(m_resourceCache);
	source->SetResource(file);
	source->SetPosition(position);
	source->SetVolume(volume);
	source->SetRelativeToListener(relativeToListener);
	source->SetLooping(false);
	source->Play();

	m_temporarySources.push_back(source);

	// Perform a cull on the next update.
	m_timeAccumulationCulling = FLT_MAX;
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

	if (m_timeAccumulationCulling >= (1.0f / SOUNDENGINE_CULLING_FREQUENCY)) {
		m_timeAccumulationCulling = 0.0f;

		// Concatenate m_sources and m_temporarySources to get a list of all sources.
		std::vector<SoundSourceImpl *> sources = m_sources;
		sources.reserve(m_sources.size() + m_temporarySources.size());
		sources.insert(sources.end(), m_temporarySources.begin(), m_temporarySources.end());

		std::vector<SoundSourceImpl *> previousActiveSources = m_activeSources;
		m_activeSources = ApplyCullingDistance(m_activeListener, sources);
		// Both containers must be sorted for std::set_difference to work.
		std::sort(m_activeSources.begin(), m_activeSources.end());

		std::vector<SoundSourceImpl *> difference;
		std::set_difference(previousActiveSources.begin(), previousActiveSources.end(), m_activeSources.begin(), m_activeSources.end(), std::inserter(difference, difference.begin()));

		// difference contains newly deactivated sources.
		for (auto iter = difference.begin(); iter != difference.end(); iter++) {
			source = (*iter);
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

			source = (*iter);
			source->LendSource(m_availableSources.top());
			m_availableSources.pop();
		}

		//printf("%i out of %i sources survived culling.\n", m_activeSources.size(), sources.size());
	}

	for (auto iter = m_sources.begin(); iter != m_sources.end(); iter++) {
		source = (*iter);
		source->Update(deltaTime);
	}

	for (size_t i = 0; i < m_temporarySources.size(); i++) {
		source = m_temporarySources[i];
		source->Update(deltaTime);

		if (!source->IsPlaying()) {
			m_temporarySources[i] = m_temporarySources.back();
			m_temporarySources.pop_back();

			if (source->HasSource()) {
				m_availableSources.push(source->RevokeSource());
			}

			for (auto iter = m_activeSources.begin(); iter != m_activeSources.end(); iter++) {
				if ((*iter) == source) {
					m_activeSources.erase(iter);
					break;
				}
			}

			delete source;
			i--;
		}
	}

	assert(alGetError() == AL_NO_ERROR);
}
