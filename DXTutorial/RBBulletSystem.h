#pragma once

#include "ComponentSystem.h"
#include "PrimitiveFactory.h"
#include "Types.h"


struct RBBulletComponent
{
	U64 transform = 0;
	Material* material;
	float cooldown = 0.33;
};


class RBBulletSystem : public ComponentSystem<RBBulletComponent>
{
public:
	void AddSystemRefs(TransformSystem* transformSystem, PrimitiveFactory* factory, InputManager* input)
	{
		m_transformSystem = transformSystem;
		m_factory = factory;
		m_input = input;
	}

	inline void Execute(float deltaTime) override
	{
		for (U32 i = 0; i < m_pool.Size(); i++)
		{
			RBBulletComponent* comp = m_pool[i];
			TransformComponent* transform = m_transformSystem->GetComponentByHandle(comp->transform);

			// check cooldown
			// TODO: keep track of elapsed per component
			static float elapsed = comp->cooldown;
			if (elapsed < comp->cooldown)
			{
				elapsed += deltaTime;
				return;
			}

			// check input
			if (!m_input->GetGamepad().GetButtonState(A_BUTTON))
			{
				return;
			}

			elapsed = 0;

			float zOffset = 5;
			float yOffset = -0.5;
			float force = 10;

			XMVECTOR forward = XMVectorSet(0, 0, 1, 1);
			forward = XMVector3Rotate(forward, transform->rotation);

			XMVECTOR position = XMVectorAdd(transform->position, forward * zOffset);
			position = XMVectorAdd(position, XMVectorSet(0, yOffset, 0, 1));

			XMVECTOR velocity = forward * force;

			m_factory->CreatePrimitive(PRIM_CUBE, 1, comp->material, XMVecToVec3(position), vec3(0), vec3(1), XMVecToVec3(velocity));
		}
	}

private:
	vec3 XMVecToVec3(XMVECTOR dxVec)
	{
		vec3 vec(
			dxVec.m128_f32[0],
			dxVec.m128_f32[1],
			dxVec.m128_f32[2]
		);

		return vec;
	}

private:
	TransformSystem* m_transformSystem;
	PrimitiveFactory* m_factory;
	InputManager* m_input;
};