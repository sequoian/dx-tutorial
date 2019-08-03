#pragma once

#include "ComponentSystem.h"
#include "TransformSystem.h"
#include "InputManager.h"
#include "MathUtility.h"
#include "Physics.h"

struct PivotCamComponent
{
	U64 hParentTransform;
	U64 hCameraTransform;
	XMVECTOR rotation;
	float length;
	float pitch;
	float yaw;
	float lookSpeed;
};

class PivotCamSystem : public ComponentSystem<PivotCamComponent>
{
public:
	bool StartUp(U32 numComponents, EntityManager& em, TransformSystem& transformSystem, InputManager& inputManager, Physics& physics)
	{
		Parent::StartUp(numComponents, em);
		m_transformSystem = &transformSystem;
		m_inputManager = &inputManager;
		m_physics = &physics;
		return true;
	}

	U64 CreateComponent(Entity e, U64 hParentTransform, U64 hCameraTransform, float length, float lookSpeed, float pitch = 0, float yaw = 0)
	{
		U64 handle = Parent::CreateComponent(e);
		PivotCamComponent* comp = GetComponentByHandle(handle);

		comp->hParentTransform = hParentTransform;
		comp->hCameraTransform = hCameraTransform;
		comp->length = length;
		comp->lookSpeed = 5;
		comp->pitch = pitch;
		comp->yaw = yaw;
		comp->rotation = Quaternion(pitch, yaw, 0);

		return handle;
	}

	inline void Execute(float deltaTime) override
	{
		for (U32 i = 0; i < m_pool.Size(); i++)
		{
			PivotCamComponent* comp = m_pool[i];
			TransformComponent* parent = m_transformSystem->GetComponentByHandle(comp->hParentTransform);
			TransformComponent* cam = m_transformSystem->GetComponentByHandle(comp->hCameraTransform);

			HandleInput(comp, deltaTime);

			// set camera position
			XMVECTOR dir = XMVector3Rotate(Vector3(0, 0, 1), comp->rotation);
			dir *= comp->length;
			cam->position = XMVectorSubtract(parent->position, dir);

			LookAtParent(comp, parent, cam);

			AdjustSpring(comp);
		}
	}

protected:
	void HandleInput(PivotCamComponent* comp, float dt)
	{
		const float lookSpeed = comp->lookSpeed;
		float pitch = comp->pitch;
		float yaw = comp->yaw;

		yaw += m_inputManager->GetGamepad().GetAxisState(GamepadAxes::RIGHT_THUMB_X) * lookSpeed * dt;
		pitch += m_inputManager->GetGamepad().GetAxisState(GamepadAxes::RIGHT_THUMB_Y) * lookSpeed * dt;

		// limit pitch
		float limit = 85;
		pitch = RadiansToDegrees(pitch);
		if (pitch > limit) pitch = limit;
		else if (pitch < -limit) pitch = -limit;
		pitch = DegreesToRadians(pitch);

		XMVECTOR xAxis = XMVectorSet(1, 0, 0, 1);
		XMVECTOR yAxis = XMVectorSet(0, 1, 0, 1);
		XMVECTOR yRot = XMQuaternionRotationAxis(yAxis, yaw);
		XMVECTOR xRot = XMQuaternionRotationAxis(xAxis, -pitch);

		comp->pitch = pitch;
		comp->yaw = yaw;
		comp->rotation = XMQuaternionMultiply(xRot, yRot);
	}

	void LookAtParent(PivotCamComponent* comp, TransformComponent* parent, TransformComponent* cam)
	{
		XMVECTOR pos, rot, scale;
		XMMATRIX lookAt = DirectX::XMMatrixLookAtLH(cam->position, parent->position, Vector3(0, 1, 0));

		// get the inverse because CameraSystem will also get the inverse. probably should fix this?
		lookAt = XMMatrixInverse(nullptr, lookAt);

		DirectX::XMMatrixDecompose(&scale, &rot, &pos, lookAt);
		cam->position = pos;
		cam->rotation = rot;
		cam->scale = scale;
	}

	void AdjustSpring(PivotCamComponent* comp)
	{
		TransformComponent* parentTransform = m_transformSystem->GetComponentByHandle(comp->hParentTransform);
		TransformComponent* camTransform = m_transformSystem->GetComponentByHandle(comp->hCameraTransform);

		XMVECTOR rayStart = parentTransform->position;
		XMVECTOR rayEnd = camTransform->position;

		// This would be more efficient to use the closest raycast function combined with collision masks to filter out triggers
		auto result = m_physics->RayCastAll(rayStart, rayEnd);
		if (result.hasHit())
		{
			RigidBody rigidBody;
			int idx = -1;
			float closestFraction = 1;

			// filter out triggers and get closest object
			for (int i = 0; i < result.m_collisionObjects.size(); ++i)
			{
				if (result.m_hitFractions[i] < closestFraction)
				{
					const btRigidBody* crb = static_cast<const btRigidBody*>(result.m_collisionObjects.at(i));
					btRigidBody* rb = const_cast<btRigidBody*>(crb);
					rigidBody = RigidBody(rb);
					if (!rigidBody.IsTrigger())
					{
						idx = i;
						closestFraction = result.m_hitFractions[i];
					}
				}
			}

			// return if it's only hit triggers
			if (idx < 0)
			{
				return;
			}

			XMVECTOR ptA = Physics::VecToDX(result.m_hitPointWorld[idx]);

			// set camera outside obstruction
			camTransform->position = ptA;
		}
	}

protected:
	TransformSystem* m_transformSystem;
	InputManager* m_inputManager;
	Physics* m_physics;
};