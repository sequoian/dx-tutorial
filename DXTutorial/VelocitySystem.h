#pragma once

#include "ComponentSystem.h"
#include <DirectXMath.h>
using namespace DirectX;


struct VelocityComponent
{
	XMVECTOR velocity = XMVectorSet(0.f, 0.f, 0.f, 1.f);
};


class VelocitySystem : public ComponentSystem<VelocityComponent>
{
public:
	inline void Execute(float deltaTime) override
	{
	}

};