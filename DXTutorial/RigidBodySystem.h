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
	void AddSystemRefs(Physics* physics)
	{
		m_physics = physics;
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