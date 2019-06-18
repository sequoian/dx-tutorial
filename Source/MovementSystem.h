#pragma once

#include "ComponentSystem.h"
#include "TransformSystem.h"
#include "PivotCamSystem.h"
#include "InputManager.h"
#include "MathUtility.h"

struct MovementComponent
{
	U64 hTransform;
	U64 hPivotCam;
	float moveSpeed;
};

class MovementSystem : public ComponentSystem<MovementComponent>
{
public:
	bool StartUp(U32 numComponents, EntityManager& em, TransformSystem& transformSystem, InputManager& input, PivotCamSystem& pivotCam)
	{
		Parent::StartUp(numComponents, em);
		m_transformSystem = &transformSystem;
		m_inputManager = &input;
		m_pivotCamSystem = &pivotCam;
		return true;
	}

	U64 CreateComponent(Entity e, U64 hTransform, U64 hPivotCam, float moveSpeed)
	{
		U64 handle = Parent::CreateComponent(e);
		MovementComponent* comp = GetComponentByHandle(handle);

		comp->hTransform = hTransform;
		comp->hPivotCam = hPivotCam;
		comp->moveSpeed = moveSpeed;

		return handle;
	}

	inline void Execute(float deltaTime) override
	{
		for (U32 i = 0; i < m_pool.Size(); i++)
		{
			MovementComponent* comp = m_pool[i];
			TransformComponent* transform = m_transformSystem->GetComponentByHandle(comp->hTransform);
			PivotCamComponent* pivotCam = m_pivotCamSystem->GetComponentByHandle(comp->hPivotCam);

			Move(transform, pivotCam->yaw, comp->moveSpeed, deltaTime);
		}
	}

protected:
	inline void Move(TransformComponent* transform, float camYaw, float moveSpeed, float dt)
	{
		// calculate move direction
		float x = m_inputManager->GetGamepad().GetAxisState(GamepadAxes::LEFT_THUMB_X);
		float z = m_inputManager->GetGamepad().GetAxisState(GamepadAxes::LEFT_THUMB_Y);

		// calculate movement vector
		XMVECTOR movement = XMVectorSet(x, 0, z, 1);

		// move relative to camera yaw
		XMVECTOR quat = XMQuaternionRotationRollPitchYaw(0, camYaw, 0);
		movement = XMVector3Rotate(movement, quat);

		// scale movement
		movement *= moveSpeed * dt;

		// set position and rotation
		transform->position += movement;
		transform->rotation = XMQuaternionRotationRollPitchYaw(0, camYaw + 180.0_rad, 0);
	}

protected:
	TransformSystem * m_transformSystem;
	InputManager* m_inputManager;
	PivotCamSystem* m_pivotCamSystem;
};