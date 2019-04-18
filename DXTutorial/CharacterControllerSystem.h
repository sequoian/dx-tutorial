#pragma once

#include "ComponentSystem.h"
#include "TransformSystem.h"
#include "Physics.h"
#include "Types.h"


struct CharacterControllerComponent
{
	U64 transform = 0;
	btKinematicCharacterController* character = nullptr;
};


class CharacterControllerSystem : public ComponentSystem<CharacterControllerComponent>
{
public:
	void AddSystemRefs(TransformSystem* transformSystem)
	{
		m_transformSystem = transformSystem;
	}

	inline void Execute(float deltaTime) override
	{
		for (U32 i = 0; i < m_pool.Size(); i++)
		{
			CharacterControllerComponent* cc = m_pool[i];
			TransformComponent* transform = m_transformSystem->GetComponentByHandle(cc->transform);

			btQuaternion rotation = Physics::QuatFromDX(transform->rotation);
			btVector3 position = Physics::VecFromDX(transform->position);
			btTransform t(rotation, position);

			cc->character->getGhostObject()->setWorldTransform(t);
		}
	}

private:
	TransformSystem* m_transformSystem;
};