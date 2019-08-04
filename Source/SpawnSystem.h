#pragma once

#include "ComponentSystem.h"
#include "Types.h"

struct SpawnComponent
{
	XMVECTOR position;
	XMVECTOR rotation;
	float camPitch;
	float camYaw;
};


class SpawnSystem : public ComponentSystem<SpawnComponent>
{
public:
	U64 CreateComponent(Entity e, XMVECTOR position, XMVECTOR rotation, float camPitch = 0, float camYaw = 0)
	{
		U64 handle = Parent::CreateComponent(e);
		SpawnComponent* comp = GetComponentByHandle(handle);

		comp->position = position;
		comp->rotation = rotation;
		comp->camPitch = camPitch;
		comp->camYaw = camYaw;

		return handle;
	}

	inline void Execute(float deltaTime) override
	{
	}

	inline void SetActiveSpawn(U64 handle)
	{
		if (m_pool.GetObjectByHandle(handle))
		{
			m_active = handle;
		}
		else
		{
			DEBUG_ERROR("Cannot set active spawn to missing spawn component");
		}
	}

	inline const SpawnComponent* GetActiveSpawn() const
	{
		return m_pool.GetObjectByHandleConst(m_active);
	}

private:
	U64 m_active = 0;
};