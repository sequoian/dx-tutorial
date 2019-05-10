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
};


class CharacterControllerSystem : public ComponentSystem<CharacterControllerComponent>
{
public:
	void AddSystemRefs(TransformSystem* transformSystem, EventBus& eventBus)
	{
		m_transformSystem = transformSystem;
		//eventBus.Subscribe(this, &CharacterControllerSystem::OnCollisionEvent);
		//std::vector<btManifoldPoint> v;
		//OnCollisionEvent(&CollisionEvent(RigidBody(), RigidBody(), 1, v));
	}

	inline void Execute(float deltaTime) override
	{
	}

private:
	TransformSystem* m_transformSystem;
};