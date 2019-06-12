#pragma once

#include "ComponentSystem.h"
#include "TransformSystem.h"
#include "Types.h"


struct RotatorComponent
{
	U64 transform = 0;
	float angle = 0;
	float speed = 0;
};


class RotatorSystem : public ComponentSystem<RotatorComponent>
{
public:
	bool StartUp(U32 numComponents, EntityManager& em, TransformSystem& transformSystem)
	{
		ComponentSystem<RotatorComponent>::StartUp(numComponents, em);

		m_transformSystem = &transformSystem;

		return true;
	}

	U64 CreateComponent(Entity e, U64 hTransform, float speed)
	{
		U64 handle = TParent::CreateComponent(e);
		RotatorComponent* comp = GetComponentByHandle(handle);
		comp->transform = hTransform;
		comp->speed = speed;

		return handle;
	}

	inline void Execute(float deltaTime) override
	{
		// hard coded axis
		XMVECTOR axis = XMVectorSet(0, 1, 0, 1);

		for (U32 i = 0; i < m_pool.Size(); i++)
		{
			RotatorComponent* comp = m_pool[i];
			TransformComponent* transform = m_transformSystem->GetComponentByHandle(comp->transform);
			comp->angle += comp->speed * deltaTime;
			transform->rotation = XMQuaternionRotationAxis(axis, comp->angle);
		}
	}

private:
	TransformSystem* m_transformSystem;
};