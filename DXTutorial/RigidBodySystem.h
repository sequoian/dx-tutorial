#pragma once

#include "ComponentSystem.h"
#include "TransformSystem.h"
#include "Physics.h"
#include "Types.h"


struct RigidBodyComponent
{
	U64 transform = 0;
	btRigidBody* body = nullptr;
};


class RigidBodySystem : public ComponentSystem<RigidBodyComponent>
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
			RigidBodyComponent* rb = m_pool[i];
			
			float m[16];
			btTransform t;
			rb->body->getMotionState()->getWorldTransform(t);
			t.getOpenGLMatrix(m);

			TransformComponent* transform = m_transformSystem->GetComponentByHandle(rb->transform);

			transform->transform = XMMATRIX(m);

			DEBUG_PRINT("rigid body");
			
		}
	}

private:
	TransformSystem* m_transformSystem;
};