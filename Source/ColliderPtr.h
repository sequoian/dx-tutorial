#pragma once

#include <btBulletDynamicsCommon.h>
#include <DirectXMath.h>

class Physics;

class ColliderPtr
{
public:
	friend class Physics;
	ColliderPtr() {};
	ColliderPtr(btCollisionShape* collider) : m_collider(collider) {};
	void SetScale(DirectX::XMVECTOR scale);
protected:
	btCollisionShape* m_collider;
};