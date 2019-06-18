#pragma once

#include "ComponentSystem.h"
#include "TransformSystem.h"
#include "InputManager.h"
#include "MathUtility.h"

struct PlatformerComponent
{
	U64 hTransform;
	float moveSpeed;
};

class PlatformerSystem : public ComponentSystem<PlatformerComponent>
{
public:
	bool StartUp(U32 numComponents, EntityManager& em, TransformSystem& transformSystem, InputManager& input)
	{
		Parent::StartUp(numComponents, em);
		m_transformSystem = &transformSystem;
		m_inputManager = &input;
		return true;
	}

	U64 CreateComponent(Entity e, U64 hTransform, float moveSpeed)
	{
		U64 handle = Parent::CreateComponent(e);
		PlatformerComponent* comp = GetComponentByHandle(handle);

		comp->hTransform = hTransform;
		comp->moveSpeed = moveSpeed;

		return handle;
	}

	inline void Execute(float deltaTime) override
	{
		for (U32 i = 0; i < m_pool.Size(); i++)
		{
			PlatformerComponent* comp = m_pool[i];
			TransformComponent* transform = m_transformSystem->GetComponentByHandle(comp->hTransform);

			Move(transform, comp->moveSpeed, deltaTime);
		}
	}

protected:
	inline void Move(TransformComponent* transform, float moveSpeed, float dt)
	{
		// calculate move direction
		float x = m_inputManager->GetGamepad().GetAxisState(GamepadAxes::LEFT_THUMB_X);
		float z = m_inputManager->GetGamepad().GetAxisState(GamepadAxes::LEFT_THUMB_Y);

		// calculate movement vector
		XMVECTOR movement = XMVectorSet(x, 0, z, 1);

		// move relative to local transform
		//movement = XMVector3Rotate(movement, transform->rotation);

		// scale movement
		movement *= moveSpeed * dt;

		// set position
		transform->position += movement;
	}

protected:
	TransformSystem * m_transformSystem;
	InputManager* m_inputManager;
};