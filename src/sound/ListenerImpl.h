#ifndef SOUND_LISTENERIMPL_H
#define SOUND_LISTENERIMPL_H

#include "Sound/Listener.h"

class ListenerImpl : public Listener
{
public:
	ListenerImpl();
	virtual ~ListenerImpl();

	virtual void SetMasterVolume(float volume);

	virtual void SetPosition(const float position[3]);
	virtual void SetOrientation(const float forward[3], const float up[3]);

	// Used for the doppler effect, should be in the same unit as the speed of sound set in SoundEngine.
	virtual void SetVelocity(const float velocity[3]);

private:
	friend class SoundEngineImpl;

	float m_volume;
	float m_position[3];
	float m_orientation[6];
	float m_velocity[3];
};

#endif
