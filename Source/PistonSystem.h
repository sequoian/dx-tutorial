#pragma once

#include "ComponentSystem.h"
#include "TransformSystem.h"
#include "Types.h"
#include "MathUtility.h"


struct PistonComponent
{
	U64 hTransform;
	XMVECTOR startPos;
	XMVECTOR endPos;
	float secToEnd;
	float secAtEnd;
	float secToStart;
	float secAtStart;
	I32 currentState;
	float secInCurrentState;
};


class PistonSystem : public ComponentSystem<PistonComponent>
{
public:
	bool StartUp(U32 numComponents, EntityManager& em, TransformSystem& transformSystem)
	{
		ComponentSystem<PistonComponent>::StartUp(numComponents, em);

		m_transformSystem = &transformSystem;

		return true;
	}

	U64 CreateComponent(Entity e, U64 hTransform, XMVECTOR startPos, XMVECTOR endPos, float secToEnd, float secToStart, float secAtEnd = 0.f, float secAtStart = 0.f)
	{
		U64 handle = Parent::CreateComponent(e);
		PistonComponent* comp = GetComponentByHandle(handle);

		comp->hTransform = hTransform;
		comp->startPos = startPos;
		comp->endPos = endPos;
		comp->secToEnd = secToEnd;
		comp->secAtEnd = secAtEnd;
		comp->secToStart = secToStart;
		comp->secAtStart = secAtStart;
		comp->currentState = MOVING_TO_END;
		comp->secInCurrentState = 0.f;

		return handle;
	}

	inline void Execute(float deltaTime) override
	{
		for (U32 i = 0; i < m_pool.Size(); i++)
		{
			PistonComponent* comp = m_pool[i];
			TransformComponent* transform = m_transformSystem->GetComponentByHandle(comp->hTransform);

			float lerp = 0;
			float excess = 0;

			switch (comp->currentState)
			{
			case MOVING_TO_END:
				comp->secInCurrentState += deltaTime;
				lerp = 0;
				excess = 0;
				if (comp->secInCurrentState >= comp->secToEnd)
				{
					lerp = comp->secToEnd;
					excess = comp->secInCurrentState - comp->secToEnd;
					comp->secInCurrentState = excess;
					if (comp->secAtEnd <= 0)
					{
						comp->currentState = MOVING_TO_START;
					}
					else
					{
						comp->currentState = STOPPED_AT_END;
					}			
				}
				else
				{
					lerp = comp->secInCurrentState;
				}
				transform->position = XMVectorLerp(comp->startPos, comp->endPos, lerp / comp->secToEnd);
				break;
			case STOPPED_AT_END:
				comp->secInCurrentState += deltaTime;
				lerp = 0;
				excess = 0;
				if (comp->secInCurrentState >= comp->secAtEnd)
				{
					lerp = comp->secAtEnd;
					excess = comp->secInCurrentState - comp->secAtEnd;
					comp->secInCurrentState = excess;
					comp->currentState = MOVING_TO_START;
				}
				break;
			case MOVING_TO_START:
				comp->secInCurrentState += deltaTime;
				lerp = 0;
				excess = 0;
				if (comp->secInCurrentState >= comp->secToStart)
				{
					lerp = comp->secToStart;
					excess = comp->secInCurrentState - comp->secToStart;
					comp->secInCurrentState = excess;
					if (comp->secAtStart <= 0)
					{
						comp->currentState = MOVING_TO_END;
					}
					else
					{
						comp->currentState = STOPPED_AT_START;
					}
				}
				else
				{
					lerp = comp->secInCurrentState;
				}

				transform->position = XMVectorLerp(comp->endPos, comp->startPos, lerp / comp->secToStart);
				break;
			case STOPPED_AT_START:
				comp->secInCurrentState += deltaTime;
				lerp = 0;
				excess = 0;
				if (comp->secInCurrentState >= comp->secAtStart)
				{
					lerp = comp->secAtStart;
					excess = comp->secInCurrentState - comp->secAtStart;
					comp->secInCurrentState = excess;
					comp->currentState = MOVING_TO_END;
				}
				break;
			default:
				DEBUG_ERROR("Invalid piston state");
			}
		}
	}

protected:
	enum PistonState
	{
		MOVING_TO_END = 0,
		STOPPED_AT_END,
		MOVING_TO_START,
		STOPPED_AT_START
	};

	TransformSystem* m_transformSystem;
};