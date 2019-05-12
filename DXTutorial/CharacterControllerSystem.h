#pragma once

#include "ComponentSystem.h"
#include "TransformSystem.h"
#include "Physics.h"
#include "Types.h"
#include "RigidBody.h"
#include "EventBus.h"
#include "VelocitySystem.h"


struct CharacterControllerComponent
{
	U64 transform = 0;
	U64 velocity = 0;
	RigidBody rigidbody;
};


class CharacterControllerSystem : public ComponentSystem<CharacterControllerComponent>
{
public:
	void AddSystemRefs(TransformSystem* transformSystem, VelocitySystem* velocitySystem, EventBus& eventBus)
	{
		m_transformSystem = transformSystem;
		m_velocitySystem = velocitySystem;
	}

	inline void Execute(float deltaTime) override
	{

		for (U32 i = 0; i < m_pool.Size(); i++)
		{
			CharacterControllerComponent* cc = m_pool[i];
			TransformComponent* transform = m_transformSystem->GetComponentByHandle(cc->transform);
			VelocityComponent* velocity = m_velocitySystem->GetComponentByHandle(cc->velocity);

			//cc->rigidbody.SetRotation(transform->rotation);
			cc->rigidbody.SetAngularVelocity(velocity->angular);
			cc->rigidbody.SetLinearVelocity(velocity->velocity);
		}
		
	}

private:
	TransformSystem* m_transformSystem;
	VelocitySystem* m_velocitySystem;
};