#pragma once

#include "ComponentSystem.h"
#include "TransformSystem.h"
#include "InputManager.h"
#include "WriteLog.h"
#include <DirectXMath.h>
using namespace std;


struct FlyCamComponent
{
	U64 transform;
	float moveSpeed;
	float sprintSpeed;
	float crawlSpeed;
	float lookSpeed;
	float pitch = 0;
	float yaw = 0;
};


class FlyCamSystem : public ComponentSystem<FlyCamComponent>
{
public:

	bool StartUp(U32 numComponents, EntityManager& em, InputManager& inputManager, TransformSystem& transformSystem)
	{
		Parent::StartUp(numComponents, em);

		m_inputManager = &inputManager;
		m_transformSystem = &transformSystem;

		return true;
	}

	U64 CreateComponent(Entity e, U64 hTransform, float moveSpeed, float sprintSpeed, float crawlspeed, float lookSpeed)
	{
		U64 handle = Parent::CreateComponent(e);
		FlyCamComponent* comp = GetComponentByHandle(handle);

		comp->transform = hTransform;
		comp->moveSpeed = moveSpeed;
		comp->sprintSpeed = sprintSpeed;
		comp->crawlSpeed = crawlspeed;
		comp->lookSpeed = lookSpeed;

		return handle;
	}

	inline void Execute(float deltaTime) override
	{
		for (int i = 0; i < m_pool.Size(); ++i)
		{
			FlyCamComponent* flycam = m_pool[i];
			TransformComponent* transform = m_transformSystem->GetComponentByHandle(flycam->transform);

			Look(transform, flycam->lookSpeed, deltaTime, flycam->pitch, flycam->yaw);
			Move(transform, flycam->moveSpeed, flycam->sprintSpeed, flycam->crawlSpeed, deltaTime);
		}
	}

private:

	inline void Move(TransformComponent* transform, float moveSpeed, float sprintSpeed, float crawlSpeed, float dt)
	{
		// calculate move direction
		float x = m_inputManager->GetGamepad().GetAxisState(GamepadAxes::LEFT_THUMB_X);
		float z = m_inputManager->GetGamepad().GetAxisState(GamepadAxes::LEFT_THUMB_Y);
		float down = m_inputManager->GetGamepad().GetButtonState(GamepadButtons::LEFT_SHOULDER) ? -1 : 0;
		float up = m_inputManager->GetGamepad().GetButtonState(GamepadButtons::RIGHT_SHOULDER) ? 1 : 0;
		float y = up + down;

		// calculate movespeed
		float ms;
		float sprint = m_inputManager->GetGamepad().GetAxisState(GamepadAxes::RIGHT_TRIGGER);
		float crawl = m_inputManager->GetGamepad().GetAxisState(GamepadAxes::LEFT_TRIGGER);
		if (sprint)
		{
			ms = sprintSpeed;
		}
		else if (crawl)
		{
			ms = crawlSpeed;
		}
		else
		{
			ms = moveSpeed;
		}

		// calculate movement vector
		XMVECTOR movement = XMVectorSet(x, y, z, 1);
		
		// move relative to local transform
		movement = XMVector3Rotate(movement, transform->rotation);

		// scale movement
		movement *= ms * dt;

		// set position
		transform->position += movement;
	}

	inline void Look(TransformComponent* transform, float lookSpeed, float dt, float &pitch, float& yaw)
	{
		float x = m_inputManager->GetGamepad().GetAxisState(GamepadAxes::RIGHT_THUMB_X) * lookSpeed * dt;
		yaw += x;
		float y = m_inputManager->GetGamepad().GetAxisState(GamepadAxes::RIGHT_THUMB_Y) * lookSpeed * dt;
		pitch += y;

		// limit pitch
		pitch = RadiansToDegrees(pitch);
		if (pitch > 90) pitch = 90;
		else if (pitch < -90) pitch = -90;
		pitch = DegreesToRadians(pitch);

		XMVECTOR xAxis = XMVectorSet(1, 0, 0, 1);
		XMVECTOR yAxis = XMVectorSet(0, 1, 0, 1);
		XMVECTOR yRot = XMQuaternionRotationAxis(yAxis, yaw);
		XMVECTOR xRot = XMQuaternionRotationAxis(xAxis, -pitch);

		transform->rotation = XMQuaternionMultiply(xRot, yRot);
	}

private:
	TransformSystem* m_transformSystem;
	InputManager* m_inputManager;
};