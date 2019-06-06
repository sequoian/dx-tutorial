#pragma once

#include "ComponentSystem.h"
#include "TransformSystem.h"
#include "WriteLog.h"
#include "Timer.h"
#include <DirectXMath.h>
using namespace DirectX;

struct DoorComponent
{
	U64 transform = 0;
	XMVECTOR startingPosition;
	XMVECTOR endingPosition;
	float secondsToMove;
	float timeTaken = 0;
	bool active;
};


class DoorSystem : public ComponentSystem<DoorComponent>
{
public:

	void AddSystemRefs(TransformSystem* transformSystem)
	{
		m_transformSystem = transformSystem;
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

private:

	TransformSystem* m_transformSystem;
};