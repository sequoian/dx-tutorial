#pragma once

#include "ComponentSystem.h"
#include "TransformSystem.h"
#include "Physics.h"
#include "Types.h"
#include "RigidBody.h"
#include "EventBus.h"


struct CharacterControllerComponent
{
	U64 transform = 0;
	RigidBody rigidBody;
};


class CharacterControllerSystem : public ComponentSystem<CharacterControllerComponent>
{
public:
	void AddSystemRefs(TransformSystem* transformSystem, EventBus& eventBus)
	{
		m_transformSystem = transformSystem;
		eventBus.Subscribe(this, &CharacterControllerSystem::OnCollisionEvent);
	}

	inline void Execute(float deltaTime) override
	{
		for (U32 i = 0; i < m_pool.Size(); i++)
		{

		}
	}

private:
	void OnCollisionEvent(CollisionEvent* collision)
	{
		Entity a = collision->entityA;
		Entity b = collision->entityB;
		if (FindComponent(a) || FindComponent(b))
			DEBUG_PRINT("character collided!");
	}

private:
	TransformSystem* m_transformSystem;
};