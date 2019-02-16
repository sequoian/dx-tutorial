#pragma once

#include "ComponentSystem.h"
#include "WriteLog.h"
#include <DirectXMath.h>
using namespace DirectX;


struct TransformComponent
{
	XMMATRIX transform = DirectX::XMMatrixIdentity();
};


class TransformSystem : public ComponentSystem<TransformComponent>
{
public:
	inline void Execute(float deltaTime) override
	{
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