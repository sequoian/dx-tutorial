#include "Types.h"
#include "Entity.h"
#include "Physics.h"
#include <DirectXMath.h>
using namespace DirectX;

class RigidBody
{
public:
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
	void SetFilterGroup(U32 group);
	void SetFilterMask(U32 mask);
	U32 GetMass();
	void SetEntity(Entity e);
	Entity GetEntity();
	bool IsDynamic();
	bool IsStatic();
	bool IsKinematic();

protected:
	btRigidBody* body;
	friend class Physics;
};