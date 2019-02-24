#pragma once

#include "ComponentSystem.h"
#include "Model.h"
#include "Material.h"

struct MeshComponent
{
	U64 transform;
	Model* model;
	Material* material;
};


class MeshSystem : public ComponentSystem<MeshComponent>
{
public:
	inline void Execute(float deltaTime) override
	{
	}

	inline U32 Size()
	{
		return m_pool.Size();
	}

	inline MeshComponent* operator[] (I32 idx)
	{
		return m_pool[idx];
	}

};