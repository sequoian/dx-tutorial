#pragma once

#include "ComponentSystem.h"
#include "Model.h"
#include "Material.h"

struct MeshComponent
{
	U64 transform;
	Model* model = nullptr;
	Material* material = nullptr;
};


class MeshSystem : public ComponentSystem<MeshComponent>
{
public:
	U64 CreateComponent(Entity e, U64 hTransform, Model* model, Material* material)
	{
		U64 handle = Parent::CreateComponent(e);
		MeshComponent* comp = GetComponentByHandle(handle);
		comp->transform = hTransform;
		comp->model = model;
		comp->material = material;

		return handle;
	}

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