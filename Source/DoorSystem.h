#pragma once

#include "ComponentSystem.h"
#include "TransformSystem.h"
#include "WriteLog.h"
#include "Timer.h"
#include "EventBus.h"
#include <DirectXMath.h>
#include "AppEvents.h"
using namespace DirectX;

struct DoorComponent
{
	U64 transform = 0;
	XMVECTOR startingPosition;
	XMVECTOR endingPosition;
	float secondsToMove;
	float timeTaken = 0;
	bool active = false;
	bool finished = false;
};


class DoorSystem : public ComponentSystem<DoorComponent>
{
public:
	bool StartUp(U32 numComponents, EntityManager& em, TransformSystem& transformSystem, EventBus& bus)
	{
		Parent::StartUp(numComponents, em);

		m_transformSystem = &transformSystem;
		bus.Subscribe(this, &DoorSystem::Activate);

		return true;
	}

	U64 CreateComponent(Entity e, U64 hTransform, XMVECTOR startPos, XMVECTOR endPos, float secToMove, bool active = false)
	{
		U64 handle = Parent::CreateComponent(e);
		DoorComponent* comp = GetComponentByHandle(handle);

		comp->transform = hTransform;
		comp->startingPosition = startPos;
		comp->endingPosition = endPos;
		comp->secondsToMove = secToMove;
		comp->active = active;
		
		return handle;
	}

	inline void Execute(float deltaTime) override
	{
		for (int i = 0; i < m_pool.Size(); ++i)
		{
			DoorComponent* door = m_pool[i];

			// if the door is inactive return
			if (!door->active)
			{
				return;
			}

			TransformComponent* transform = m_transformSystem->GetComponentByHandle(door->transform);

			door->timeTaken += deltaTime;

			// if the door is in it's final position, clamp time taken and deactivate door
			if (door->timeTaken >= door->secondsToMove)
			{
				door->active = false;
				door->finished = true;
				door->timeTaken = door->secondsToMove;
			}
			
			// move the door based on the lerp value of time taken
			transform->position = XMVectorLerp(door->startingPosition, door->endingPosition, door->timeTaken / door->secondsToMove);
		}
	}

	inline DoorComponent* operator[] (I32 idx)
	{
		return m_pool[idx];
	}

	void Activate(OpenDoorEvent* doorEvent)
	{
		DoorComponent* comp = FindComponent(doorEvent->door);
		if (!comp->active && !comp->finished)
		{
			comp->active = true;
		}
	}

private:

	TransformSystem* m_transformSystem;
};