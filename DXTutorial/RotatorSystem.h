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
	void AddSystemRefs(TransformSystem* transformSystem)
	{
		m_transformSystem = transformSystem;
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