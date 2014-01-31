#include "shared/util.h"
#include "shared/Vec3.h"
#include "Sound/SoundEngine.h"

#include "Listener.h"
#include "Entity.h"

// Must be included last!
#include "shared/debug.h"

static void MatrixFromEulerAngles(float output[4][4], float yaw, float pitch, float roll)
{
        float tmp_ch = cosf(yaw);
        float tmp_sh = sinf(yaw);
        float tmp_cp = cosf(pitch);
        float tmp_sp = sinf(pitch);
        float tmp_cb = cosf(roll);
        float tmp_sb = sinf(roll);

        output[0][0] = tmp_ch * tmp_cb + tmp_sh * tmp_sp * tmp_sb;
        output[0][1] = tmp_sb * tmp_cp;
        output[0][2] = -tmp_sh * tmp_cb + tmp_ch * tmp_sp * tmp_sb;
        output[0][3] = 0.0f;
        output[1][0] = -tmp_ch * tmp_sb + tmp_sh * tmp_sp * tmp_cb;
        output[1][1] = tmp_cb * tmp_cp;
        output[1][2] = tmp_sb * tmp_sh + tmp_ch * tmp_sp * tmp_cb;
        output[1][3] = 0.0f;
        output[2][0] = tmp_sh * tmp_cp;
        output[2][1] = -tmp_sp;
        output[2][2] = tmp_ch * tmp_cp;
        output[2][3] = 0.0f;
        output[3][0] = 0.0f;
        output[3][1] = 0.0f;
        output[3][2] = 0.0f;
        output[3][3] = 0.0f;
}

ListenerComponent::ListenerComponent() : Component("Listener")
{ 
}

ListenerComponent::~ListenerComponent()
{
}

void ListenerComponent::addedToEntity()
{
	Entity *owner = getOwner();
	m_listener = owner->getModules()->sound->CreateListener();

	requestMessage("ActivateListener", &ListenerComponent::activateListener);
}

void ListenerComponent::removeFromEntity()
{
	Entity *owner = getOwner();
	owner->getModules()->sound->DestroyListener(m_listener);
}

void ListenerComponent::update(float deltaTime)
{
	Vec3 position = getEntityPos();
	Vec3 rotation = getEntityRot();
	float rotationMatrix[4][4];
	MatrixFromEulerAngles(rotationMatrix, DegreesToRadians(rotation.X), DegreesToRadians(rotation.Y), DegreesToRadians(rotation.Z));

	float forward[3] = {rotationMatrix[2][0], rotationMatrix[2][1], -rotationMatrix[2][2]};
	float up[3] = {0.0f, 1.0f, 0.0f};

	m_listener->SetPosition(&position.X);
	m_listener->SetOrientation(forward, up);
}

void ListenerComponent::activateListener(Message const& msg)
{
	Entity *owner = getOwner();
	owner->getModules()->sound->SetActiveListener(m_listener);
}

void ListenerComponent::setVolume(float volume)
{
	m_listener->SetMasterVolume(volume);
}
