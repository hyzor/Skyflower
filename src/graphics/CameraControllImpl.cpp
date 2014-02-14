#include "CameraControllImpl.h"
#include <DirectXMath.h>

// Must be included last!
#include "shared/debug.h"

using namespace DirectX;

XMMATRIX rotate;

template <typename T>
static T Lerp(T a, T b, float amount)
{
	return a + (b - a) * amount;
}
CameraControllImpl::CameraControllImpl(Camera *c)
{
	this->camera = c;
	offset = MIN_ZOOM;
	yaw = 0;
	pitch = 0;
	targetPitch = 0;
	targetYaw = 0;
	targetY = 0.0f;

	targetY = 0.0f;
	mouseSense = 0.3f+2*0.5f;
	inverted = false;
	targetZoom = offset;
	camera->LookAt(Vec3::Zero());
	Vec3 look(camera->GetLook().x, camera->GetLook().y, camera->GetLook().z);
}

CameraControllImpl::~CameraControllImpl(){}

void CameraControllImpl::Update(float dt)
{
	if (target != Vec3::Zero())
	{
		o.Y = sin(pitch);
		o.X = cos(yaw)*cos(pitch);
		o.Z = sin(yaw)*cos(pitch);
		o = o.Normalize();

		pitch = Lerp(pitch, targetPitch, dt * 8*mouseSense);
		yaw = Lerp(yaw, targetYaw, dt * 8*mouseSense);

		
		if (std::abs(targetY - target.Y) > MAX_CAMERA_DISTANCE)
		{
			if (targetY < target.Y)
			{
				targetY = target.Y - MAX_CAMERA_DISTANCE;
			}
			else 
				targetY = target.Y + MAX_CAMERA_DISTANCE;
		}
			

		targetY = Lerp(targetY, target.Y, 2 * dt);
		target.Y = targetY;
		 
		offset = Lerp(offset, targetZoom, 3 * dt);
		camera->LookAt(target);
		SetPosition(Vec3(target + (o*offset)));
	}
	target = Vec3::Zero();
}

void CameraControllImpl::SetDirection(Vec3 direction)
{
	this->camera->SetDirection(direction);
}

Vec3 CameraControllImpl::GetPosition() const
{
	Vec3 pos;
	pos.X = camera->GetPosition().x;
	pos.Y = camera->GetPosition().y;
	pos.Z = camera->GetPosition().z;
	return pos;
}

void CameraControllImpl::SetPosition(Vec3 pos)
{
	camera->SetPosition(XMFLOAT3(pos.X, pos.Y, pos.Z));
}

Vec3 CameraControllImpl::GetLook()
{
	return o*-1.0f;
}

Vec3 CameraControllImpl::GetRight() const
{
	Vec3 right;
	right.X = camera->GetRight().x;
	right.Y = camera->GetRight().y;
	right.Z = camera->GetRight().z;
	return right;
}

Vec3 CameraControllImpl::GetUp() const
{
	Vec3 up;
	up.X = camera->GetUp().x;
	up.Y = camera->GetUp().y;
	up.Z = camera->GetUp().z;
	return up;
}

float CameraControllImpl::GetYaw() const
{
	return yaw;
}

float CameraControllImpl::GetPitch() const 
{
	return pitch;
}

void CameraControllImpl::Follow(Vec3 target)
{
	this->target = target;
	this->target.Y += 10; // Place the camera slightly over the player
}

void CameraControllImpl::SetOffset(float offset)
{
	this->offset = offset;
}

void CameraControllImpl::onMouseMove(float mouseX, float mouseY)
{
	if (targetYaw - yaw < 1 && targetYaw - yaw > -1)
	{
		if (inverted)
			targetPitch -= (mouseY / 250)*mouseSense;
		else
			targetPitch += (mouseY / 250)*mouseSense;

		targetYaw -= (mouseX / 350)*mouseSense;

	}
	if (targetPitch > 1)
		targetPitch = 1;
	else if (targetPitch < -0.1f)
		targetPitch = -0.1f;  

}

void CameraControllImpl::Zoom(float d, float speed)
{
	if ((d < 0 && targetZoom < MIN_ZOOM) || (d > 0 && targetZoom > MAX_ZOOM))
		targetZoom -= d * speed;
}

void CameraControllImpl::Rotate(float yaw, float pitch)
{
	camera->Rotate(yaw, pitch);
}

void CameraControllImpl::SetInverted(bool invert)
{
	inverted = invert;
}

bool CameraControllImpl::GetInverted() const
{
	return inverted;
}

void CameraControllImpl::SetMouseSense(float sensitivity)
{
	this->mouseSense = sensitivity;
}

float CameraControllImpl::GetMouseSense()const
{
	return mouseSense;
}

