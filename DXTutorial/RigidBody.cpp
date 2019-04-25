#include "RigidBody.h"

void RigidBody::SetTransform(XMMATRIX transform)
{
	//body->setWorldTransform();
}


void RigidBody::SetTransform(XMVECTOR position, XMVECTOR rotation)
{
	btTransform t(Physics::QuatFromDX(rotation), Physics::VecFromDX(position));
	body->getMotionState()->setWorldTransform(t);
}


XMMATRIX RigidBody::GetTransform()
{
	btTransform t;
	body->getMotionState()->getWorldTransform(t);

	return Physics::MatToDX(t);
}


void RigidBody::SetPosition(XMVECTOR position)
{
}


XMVECTOR RigidBody::GetPosition()
{

}


void RigidBody::SetRotation(XMVECTOR rotation)
{

}


XMVECTOR RigidBody::GetRotation()
{

}


void RigidBody::SetLinearVelocity(XMVECTOR velocity)
{

}


XMVECTOR RigidBody::GetLinearVelocity()
{

}


void RigidBody::SetAngularVelocity(XMVECTOR velocity)
{

}


XMVECTOR RigidBody::GetAngularVelocity()
{

}


void RigidBody::SetFilterGroup(U32 group)
{

}


void RigidBody::SetFilterMask(U32 mask)
{

}


U32 RigidBody::GetMass()
{

}


void RigidBody::SetEntity(Entity e)
{

}


Entity RigidBody::GetEntity()
{

}


bool RigidBody::IsDynamic()
{

}


bool RigidBody::IsStatic()
{

}


bool RigidBody::IsKinematic()
{

}