#pragma once

#include "ComponentSystem.h"
#include "TransformSystem.h"
#include "VelocitySystem.h"
#include "InputManager.h"
#include "WriteLog.h"
#include <DirectXMath.h>
using namespace std;


struct FlyCamComponent
{
	U64 transform;
	U64 velocity;
	float moveSpeed;
	float sprintSpeed;
	float crawlSpeed;
	float lookSpeed;
};


class FlyCamSystem : public ComponentSystem<FlyCamComponent>
{
public:
	void AddSystemRefs(InputManager* inputManager, TransformSystem* transformSystem, VelocitySystem* velocitySystem)
	{
		m_inputManager = inputManager;
		m_transformSystem = transformSystem;
		m_velocitySystem = velocitySystem;
	}

	inline void Execute(float deltaTime) override
	{
		for (int i = 0; i < m_pool.Size(); ++i)
		{
			FlyCamComponent* flycam = m_pool[i];
			TransformComponent* transform = m_transformSystem->GetComponentByHandle(flycam->transform);
			VelocityComponent* velocity = m_velocitySystem->GetComponentByHandle(flycam->velocity);

			Look(transform, flycam->lookSpeed, deltaTime);

			MoveByVelocity(transform, velocity, flycam->moveSpeed, flycam->sprintSpeed, flycam->crawlSpeed, deltaTime);

			//Move(transform, flycam->moveSpeed, flycam->sprintSpeed, flycam->crawlSpeed, deltaTime);
		}
	}

private:
	inline void MoveByVelocity(TransformComponent* transform, VelocityComponent* velocity, float moveSpeed, float sprintSpeed, float crawlSpeed, float dt)
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

		// set velocity
		velocity->velocity = movement;
	}

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

	inline void Look(TransformComponent* transform, float lookSpeed, float dt)
	{
		float x = m_inputManager->GetGamepad().GetAxisState(GamepadAxes::RIGHT_THUMB_X) * lookSpeed * dt;
		float y = m_inputManager->GetGamepad().GetAxisState(GamepadAxes::RIGHT_THUMB_Y) * lookSpeed * dt;

		XMVECTOR xAxis = XMVectorSet(1, 0, 0, 1);
		XMVECTOR yAxis = XMVectorSet(0, 1, 0, 1);
		XMVECTOR yRot = XMQuaternionRotationAxis(yAxis, x);
		XMVECTOR xRot = XMQuaternionRotationAxis(xAxis, -y);

		transform->rotation = XMQuaternionMultiply(XMQuaternionMultiply(xRot, transform->rotation), yRot);
	}

private:
	TransformSystem* m_transformSystem;
	VelocitySystem* m_velocitySystem;
	InputManager* m_inputManager;
};