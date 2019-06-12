#pragma once

#pragma once

#include "ComponentSystem.h"
#include "Physics.h"
#include "Types.h"


struct RigidBodyComponent
{
	RigidBody body;
};


class RigidBodySystem : public ComponentSystem<RigidBodyComponent>
{
public:
	bool StartUp(U32 numComponents, EntityManager& em, Physics& physics)
	{
		ComponentSystem<RigidBodyComponent>::StartUp(numComponents, em);

		m_physics = &physics;

		return true;
	}

	U64 CreateComponent(Entity e, RigidBody rigidBody)
	{
		U64 handle = TParent::CreateComponent(e);
		RigidBodyComponent* comp = GetComponentByHandle(handle);
		comp->body = rigidBody;

		return handle;
	}

	inline void Execute(float deltaTime) override
	{
		for (U32 i = 0; i < m_pool.Size(); i++)
		{
		}
	}

	inline void DestroyComponent(Entity e) override
	{
		auto itr = m_entityMap.find(e.id);
		if (itr != m_entityMap.end())
		{
			// destroy rigid body
			RigidBodyComponent* comp = GetComponentByHandle(itr->second);
			m_physics->DestroyRigidBody(comp->body);

			// destroy component
			m_pool.DestroyObject(itr->second);

			// remove entity from map
			m_entityMap.erase(itr);
		}
	}

private:
	Physics* m_physics;
};