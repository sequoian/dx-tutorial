#pragma once

#include "ComponentSystem.h"
#include "VelocitySystem.h"
#include "LegCastSystem.h"
#include "MathUtility.h"
#include "VelocitySystem.h"
#include "InputManager.h"

struct JumpComponent
{
	U64 hVelocity;
	U64 hLegCast;
	float impulse;
	bool heldPrevFrame;
};

class JumpSystem : public ComponentSystem<JumpComponent>
{
public:
	bool StartUp(U32 numComponents, EntityManager& em, VelocitySystem& vs, LegCastSystem& lcs, InputManager& input)
	{
		Parent::StartUp(numComponents, em);
		m_velocitySystem = &vs;
		m_legCastSystem = &lcs;
		m_inputManager = &input;
		return true;
	}

	U64 CreateComponent(Entity e, U64 hVelocity, U64 hLegCast, float impulse)
	{
		U64 handle = Parent::CreateComponent(e);
		JumpComponent* comp = GetComponentByHandle(handle);

		comp->hVelocity = hVelocity;
		comp->hLegCast = hLegCast;
		comp->impulse = impulse;
		comp->heldPrevFrame = false;

		return handle;
	}

	inline void Execute(float deltaTime) override
	{
		for (U32 i = 0; i < m_pool.Size(); i++)
		{
			JumpComponent* comp = m_pool[i];
			VelocityComponent* velocity = m_velocitySystem->GetComponentByHandle(comp->hVelocity);
			LegCastComponent* legCast = m_legCastSystem->GetComponentByHandle(comp->hLegCast);

			bool buttonHeld = m_inputManager->GetGamepad().GetButtonState(GamepadButtons::A_BUTTON);

			if (buttonHeld && legCast->grounded && !comp->heldPrevFrame)
			{
				velocity->velocity.m128_f32[1] = comp->impulse;
			}

			comp->heldPrevFrame = buttonHeld;
		}
	}

protected:
	VelocitySystem* m_velocitySystem;
	LegCastSystem* m_legCastSystem;
	InputManager* m_inputManager;
};