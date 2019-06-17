#pragma once

#include "ComponentSystem.h"
#include "TransformSystem.h"
#include "MathUtility.h"

struct PivotCamComponent
{
	U64 hParentTransform;
	U64 hCameraTransform;
	XMVECTOR rotation;
	float length;
	float maxLength;
};


class PivotCamSystem : public ComponentSystem<PivotCamComponent>
{
public:
	bool StartUp(U32 numComponents, EntityManager& em, TransformSystem& transformSystem)
	{
		TParent::StartUp(numComponents, em);
		m_transformSystem = &transformSystem;
		return true;
	}

	U64 CreateComponent(Entity e, U64 hParentTransform, U64 hCameraTransform, XMVECTOR rotation, float length)
	{
		U64 handle = TParent::CreateComponent(e);
		PivotCamComponent* comp = GetComponentByHandle(handle);

		comp->hParentTransform = hParentTransform;
		comp->hCameraTransform = hCameraTransform;
		comp->rotation = rotation;
		comp->length = length;
		comp->maxLength = length;

		return handle;
	}

	inline void Execute(float deltaTime) override
	{
		for (U32 i = 0; i < m_pool.Size(); i++)
		{
			PivotCamComponent* comp = m_pool[i];
			TransformComponent* parent = m_transformSystem->GetComponentByHandle(comp->hParentTransform);
			TransformComponent* cam = m_transformSystem->GetComponentByHandle(comp->hCameraTransform);

			// set camera position
			XMVECTOR pivot = XMVectorSubtract(parent->position, Vector3(0, 0, 1) * comp->length);
			cam->position = XMVector3Rotate(pivot, comp->rotation);

			// camera looks at parent
			XMVECTOR pos, rot, scale;
			XMMATRIX lookAt = DirectX::XMMatrixLookAtLH(cam->position, parent->position, Vector3(0, 1, 0));
			
			// get the inverse because CameraSystem will also get the inverse
			lookAt = XMMatrixInverse(nullptr, lookAt);

			DirectX::XMMatrixDecompose(&scale, &rot, &pos, lookAt);
			cam->position = pos;
			cam->rotation = rot;
			cam->scale = scale;
		}
	}

private:
	TransformSystem* m_transformSystem;
};