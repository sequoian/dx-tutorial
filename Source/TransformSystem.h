#pragma once

#include "ComponentSystem.h"
#include "WriteLog.h"
#include <DirectXMath.h>
using namespace DirectX;


struct TransformComponent
{
	XMVECTOR position;
	XMVECTOR rotation;
	XMVECTOR scale;
	XMMATRIX world;
};


class TransformSystem : public ComponentSystem<TransformComponent>
{
public:
	U64 CreateComponent(Entity e, XMVECTOR position, XMVECTOR rotation = XMQuaternionIdentity(), 
						XMVECTOR scale = XMVectorSet(1, 1, 1, 1))
	{
		U64 handle = Parent::CreateComponent(e);
		TransformComponent* comp = GetComponentByHandle(handle);
		comp->position = position;
		comp->rotation = rotation;
		comp->scale = scale;

		return handle;
	}

	inline void Execute(float deltaTime) override
	{
		for (U32 i = 0; i < m_pool.Size(); i++)
		{
			TransformComponent* comp = m_pool[i];
			XMMATRIX scale = XMMatrixScalingFromVector(comp->scale);
			XMMATRIX rotation = XMMatrixRotationQuaternion(comp->rotation);
			XMMATRIX position = XMMatrixTranslationFromVector(comp->position);

			comp->world = scale * rotation * position;
		}
	}

	inline U32 Size()
	{
		return m_pool.Size();
	}

	inline TransformComponent* operator[] (I32 idx)
	{
		return m_pool[idx];
	}

};