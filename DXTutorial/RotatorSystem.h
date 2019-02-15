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
		deltaTime = 1.0 / 60.0; // for testing
		for (U32 i = 0; i < m_pool.Size(); i++)
		{
			RotatorComponent* comp = m_pool[i];
			TransformComponent* transform = m_transformSystem->GetComponentByHandle(comp->transform);
			comp->angle += comp->speed * deltaTime;
			transform->transform *= DirectX::XMMatrixRotationY(comp->angle);
		}
	}

private:
	TransformSystem* m_transformSystem;
};