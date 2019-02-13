#pragma once

#include "ComponentSystem.h"
#include "WriteLog.h"
#include <DirectXMath.h>
using namespace DirectX;


struct TransformComponent
{
	TransformComponent()
	{
		transform = XMMatrixIdentity();
	}

	XMMATRIX transform;
};


class TransformSystem : public ComponentSystem<TransformComponent>
{
public:
	void Execute(float deltaTime) override
	{
		for (U32 i = 0; i < m_pool.Size(); i++)
		{
			TransformComponent* t = m_pool[i];
			DEBUG_PRINT("Transform %d");
		}
		DEBUG_PRINT("------------");
	}
};