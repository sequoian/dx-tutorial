#pragma once

#include "ComponentSystem.h"
#include "TransformSystem.h"
#include "PivotCamSystem.h"
#include "InputManager.h"
#include "MathUtility.h"
#include "VelocitySystem.h"
#include <math.h>

struct MovementComponent
{
	U64 hTransform;
	U64 hVelocity;
	U64 hPivotCam;
	U64 hLegCast;
	float moveSpeed;
};

class MovementSystem : public ComponentSystem<MovementComponent>
{
public:
	bool StartUp(U32 numComponents, EntityManager& em, TransformSystem& transformSystem, InputManager& input, PivotCamSystem& pivotCam, VelocitySystem& vs, LegCastSystem& legCast)
	{
		Parent::StartUp(numComponents, em);
		m_transformSystem = &transformSystem;
		m_inputManager = &input;
		m_pivotCamSystem = &pivotCam;
		m_velocitySystem = &vs;
		m_legCastSystem = &legCast;
		return true;
	}

	U64 CreateComponent(Entity e, U64 hTransform, U64 hPivotCam, U64 hVelocity, U64 hLegCast, float moveSpeed)
	{
		U64 handle = Parent::CreateComponent(e);
		MovementComponent* comp = GetComponentByHandle(handle);

		comp->hTransform = hTransform;
		comp->hPivotCam = hPivotCam;
		comp->moveSpeed = moveSpeed;
		comp->hVelocity = hVelocity;
		comp->hLegCast = hLegCast;

		return handle;
	}

	inline void Execute(float deltaTime) override
	{
		for (U32 i = 0; i < m_pool.Size(); i++)
		{
			MovementComponent* comp = m_pool[i];
			TransformComponent* transform = m_transformSystem->GetComponentByHandle(comp->hTransform);
			PivotCamComponent* pivotCam = m_pivotCamSystem->GetComponentByHandle(comp->hPivotCam);
			VelocityComponent* velocity = m_velocitySystem->GetComponentByHandle(comp->hVelocity);
			LegCastComponent* legCast = m_legCastSystem->GetComponentByHandle(comp->hLegCast);

			Move(transform, velocity, pivotCam->yaw, comp->moveSpeed, legCast->groundNormal, legCast->grounded, deltaTime);
		}
	}

protected:
	inline void Move(TransformComponent* transform, VelocityComponent* velocity, float camYaw, float moveSpeed, XMVECTOR groundNormal, bool grounded, float dt)
	{
		// query input
		float xInput = m_inputManager->GetGamepad().GetAxisState(GamepadAxes::LEFT_THUMB_X);
		float zInput = m_inputManager->GetGamepad().GetAxisState(GamepadAxes::LEFT_THUMB_Y);

		// separate xz velocity from y vector
		XMVECTOR xzVel = Vector3(velocity->velocity.m128_f32[0], 0, velocity->velocity.m128_f32[2]);
		XMVECTOR yVel = Vector3(0, velocity->velocity.m128_f32[1], 0);

		// handle deceleration
		xzVel *= 0.9;

		if (xInput != 0 || zInput != 0)
		{
			// set movement vector based on input
			XMVECTOR movement = XMVectorSet(xInput, 0, zInput, 1);

			// rotate movement relative to camera yaw
			XMVECTOR quat = XMQuaternionRotationRollPitchYaw(0, camYaw, 0);
			movement = XMVector3Rotate(movement, quat);

			// rotate character so they point toward their velocity
			XMVECTOR pos, rot, scale;
			XMMATRIX lookAt = DirectX::XMMatrixLookAtLH(transform->position, XMVectorAdd(transform->position, movement), Vector3(0, 1, 0));
			lookAt = XMMatrixInverse(nullptr, lookAt);
			DirectX::XMMatrixDecompose(&scale, &rot, &pos, lookAt);
			// need to turn character around
			transform->rotation = XMQuaternionMultiply(rot, XMQuaternionRotationRollPitchYaw(0, 180.0_rad, 0));

			// project movement along ground slope
			float dot = XMVector3Dot(groundNormal, movement).m128_f32[0];
			XMVECTOR parallel = dot * groundNormal;
			XMVECTOR perpendicular = XMVectorSubtract(movement, parallel);
			movement = perpendicular;

			// scale based on movespeed
			movement *= moveSpeed * dt;

			// add movement to velocity
			xzVel += movement;
		}

		// recombine xyz velocity
		velocity->velocity = XMVectorAdd(xzVel, yVel);
	}

protected:
	TransformSystem * m_transformSystem;
	InputManager* m_inputManager;
	PivotCamSystem* m_pivotCamSystem;
	VelocitySystem* m_velocitySystem;
	LegCastSystem* m_legCastSystem;
};