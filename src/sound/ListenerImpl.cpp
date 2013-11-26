#include <AL/al.h>

#include "ListenerImpl.h"

ListenerImpl::ListenerImpl()
{
}

ListenerImpl::~ListenerImpl()
{
}

void ListenerImpl::SetMasterVolume(float volume)
{
	alListenerf(AL_GAIN, volume);
}

void ListenerImpl::SetPosition(const float position[3])
{
	alListenerfv(AL_POSITION, position);
}

void ListenerImpl::SetOrientation(const float forward[3], const float up[3])
{
	float values[6] = {forward[0], forward[1], forward[2], up[0], up[1], up[2]};
	alListenerfv(AL_ORIENTATION, values);
}

void ListenerImpl::SetVelocity(const float velocity[3])
{
	alListenerfv(AL_VELOCITY, velocity);
}
