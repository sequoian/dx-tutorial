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

private:
	Physics* m_physics;
};