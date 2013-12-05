#include <AL/al.h>

#include "ListenerImpl.h"

ListenerImpl::ListenerImpl()
{
	m_volume = 1.0f;
	m_velocity[0] = 0.0f;
	m_velocity[1] = 0.0f;
	m_velocity[2] = 0.0f;
}

ListenerImpl::~ListenerImpl()
{
}

void ListenerImpl::SetMasterVolume(float volume)
{
	m_volume = volume;
}

void ListenerImpl::SetPosition(const float position[3])
{
	m_position[0] = position[0];
	m_position[1] = position[1];
	m_position[2] = position[2];
}

void ListenerImpl::SetOrientation(const float forward[3], const float up[3])
{
	m_orientation[0] = forward[0];
	m_orientation[1] = forward[1];
	m_orientation[2] = forward[2];
	m_orientation[3] = up[0];
	m_orientation[4] = up[1];
	m_orientation[5] = up[2];
}

void ListenerImpl::SetVelocity(const float velocity[3])
{
	m_velocity[0] = velocity[0];
	m_velocity[1] = velocity[1];
	m_velocity[2] = velocity[2];
}

void ListenerImpl::ApplyState()
{
	alListenerf(AL_GAIN, m_volume);
	alListenerfv(AL_POSITION, m_position);
	alListenerfv(AL_ORIENTATION, m_orientation);
	alListenerfv(AL_VELOCITY, m_velocity);
}
