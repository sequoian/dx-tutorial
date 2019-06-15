#pragma once

#include "ComponentSystem.h"
#include "PrimitiveFactory.h"
#include "Types.h"
#include "RBBulletSystem.h"


struct RBGunComponent
{
	U64 transform = 0;
	Material* material;
	float cooldown = 0.33;
};


class RBGunSystem : public ComponentSystem<RBGunComponent>
{
public:

	bool StartUp(U32 numComponents, EntityManager& em, TransformSystem& transformSystem, PrimitiveFactory& factory,
		InputManager& input, EventBus& bus, RBBulletSystem& bullet)
	{
		ComponentSystem<RBGunComponent>::StartUp(numComponents, em);

		m_transformSystem = &transformSystem;
		m_factory = &factory;
		m_input = &input;
		m_bulletSystem = &bullet;

		return true;
	}

	U64 CreateComponent(Entity e, U64 hTransform, Material* material, float cooldown)
	{
		U64 handle = TParent::CreateComponent(e);
		RBGunComponent* comp = GetComponentByHandle(handle);
		comp->transform = hTransform;
		comp->material = material;
		comp->cooldown = cooldown;

		return handle;
	}

	inline void Execute(float deltaTime) override
	{
		for (U32 i = 0; i < m_pool.Size(); i++)
		{
			RBGunComponent* comp = m_pool[i];
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
			float force = 15;

			XMVECTOR forward = XMVectorSet(0, 0, 1, 1);
			forward = XMVector3Rotate(forward, transform->rotation);

			XMVECTOR position = XMVectorAdd(transform->position, forward * zOffset);
			position = XMVectorAdd(position, XMVectorSet(0, yOffset, 0, 1));

			XMVECTOR velocity = forward * force;

			Entity e = m_factory->CreatePrimitive(PRIM_SPHERE, 1, comp->material, position, Vector3(0), Vector3(1), velocity);

			m_bulletSystem->CreateComponent(e);
		}
	}

private:
	TransformSystem* m_transformSystem;
	PrimitiveFactory* m_factory;
	InputManager* m_input;
	RBBulletSystem* m_bulletSystem;
};