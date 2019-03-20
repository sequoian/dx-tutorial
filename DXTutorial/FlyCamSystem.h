#pragma once

#include "ComponentSystem.h"
#include "TransformSystem.h"
#include "InputManager.h"
#include "WriteLog.h"


struct FlyCamComponent
{
	U64 transform;
	float moveSpeed;
	float sprintSpeed;
	float crawlSpeed;
	float lookSpeed;
};


class FlyCamSystem : public ComponentSystem<FlyCamComponent>
{
public:
	void AddSystemRefs(InputManager* inputManager, TransformSystem* transformSystem)
	{
		m_inputManager = inputManager;
		m_transformSystem = transformSystem;
	}

	inline void Execute(float deltaTime) override
	{
		for (int i = 0; i < m_pool.Size(); ++i)
		{
			FlyCamComponent* flycam = m_pool[i];
			TransformComponent* transform = m_transformSystem->GetComponentByHandle(flycam->transform);

			Look(transform->world, flycam->lookSpeed, deltaTime);

			Move(transform->world, flycam->moveSpeed, flycam->sprintSpeed, flycam->crawlSpeed, deltaTime);
		}
	}

private:
	inline void Move(XMMATRIX& transform, float moveSpeed, float sprintSpeed, float crawlSpeed, float dt)
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
		XMVECTOR movement = XMVectorSet(x, 0, z, 1);
		
		// move relative to local transform
		movement = XMVector3Rotate(movement, XMQuaternionRotationMatrix(transform));

		// scale movement
		movement *= ms * dt;

		// set transform
		transform *= XMMatrixTranslationFromVector(movement);

		// add additional y movement
		transform *= XMMatrixTranslationFromVector(XMVectorSet(0, y * ms * dt, 0, 1));
	}

	inline void Look(XMMATRIX& transform, float lookSpeed, float dt)
	{
		float x = m_inputManager->GetGamepad().GetAxisState(GamepadAxes::RIGHT_THUMB_X) * lookSpeed * dt;
		float y = m_inputManager->GetGamepad().GetAxisState(GamepadAxes::RIGHT_THUMB_Y) * lookSpeed * dt;

		XMVECTOR trans = transform.r[3];
		XMMATRIX yRot = XMMatrixRotationY(x);
		XMMATRIX xRot = XMMatrixRotationX(-y);

		transform = xRot * transform * yRot;
		transform.r[3] = trans;
	}

private:
	TransformSystem* m_transformSystem;
	InputManager* m_inputManager;
};