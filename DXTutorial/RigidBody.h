#pragma once

#include "Types.h"
#include "Entity.h"
#include "btBulletDynamicsCommon.h"
#include <DirectXMath.h>
using namespace DirectX;

class Physics;

class RigidBody
{
public:
	friend class Physics;
	RigidBody() {};
	RigidBody(btRigidBody* body);
	~RigidBody();
	void SetTransform(XMMATRIX transform);
	void SetTransform(XMVECTOR position, XMVECTOR rotation);
	XMMATRIX GetTransform();
	void SetPosition(XMVECTOR position);
	XMVECTOR GetPosition();
	void SetRotation(XMVECTOR rotation);
	XMVECTOR GetRotation();
	void SetLinearVelocity(XMVECTOR velocity);
	XMVECTOR GetLinearVelocity();
	void SetAngularVelocity(XMVECTOR velocity);
	XMVECTOR GetAngularVelocity();
	void SetEntity(Entity e);
	Entity GetEntity();
	bool IsDynamic();
	bool IsStatic();
	bool IsKinematic();
	void SetGravity(float gravity);
	bool IsTrigger();

protected:
	btRigidBody* m_body;
};