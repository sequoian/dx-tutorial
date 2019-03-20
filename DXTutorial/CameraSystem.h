#pragma once

#include "ComponentSystem.h"
#include "TransformSystem.h"
#include "SampleWindow.h"
#include "WriteLog.h"
#include <DirectXMath.h>
using namespace DirectX;

struct CameraComponent
{
	U64 transform = 0;
	float nearZ = 0;
	float farZ = 0;
	float fov = 0;
	XMMATRIX viewProjMatrix;
};


class CameraSystem : public ComponentSystem<CameraComponent>
{
public:
	void AddSystemRefs(TransformSystem* transformSystem, SampleWindow* window)
	{
		m_transformSystem = transformSystem;
		m_window = window;
	}

	inline void Execute(float deltaTime) override
	{
		U32 screenWidth = m_window->GetScreenWidth();
		U32 screenHeight = m_window->GetScreenHeight();

		for (int i = 0; i < m_pool.Size(); ++i)
		{
			CameraComponent* camera = m_pool[i];
			const TransformComponent* transform = m_transformSystem->GetComponentByHandleConst(camera->transform);
			XMMATRIX view = XMMatrixInverse(nullptr, transform->world);
			XMMATRIX proj = XMMatrixPerspectiveFovLH(camera->fov, screenWidth / float(screenHeight), camera->nearZ, camera->farZ);
			camera->viewProjMatrix = XMMatrixMultiply(view, proj);
		}
	}

	inline CameraComponent* operator[] (I32 idx)
	{
		return m_pool[idx];
	}

private:

	TransformSystem* m_transformSystem;
	SampleWindow* m_window;
};