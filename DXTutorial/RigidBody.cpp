#include "RigidBody.h"

void RigidBody::SetTransform(XMMATRIX transform)
{
	body->getMotionState()->setWorldTransform(Physics::MatFromDX(transform));
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
	btTransform t;
	body->getMotionState()->getWorldTransform(t);
	t.setOrigin(Physics::VecFromDX(position));
	body->getMotionState()->setWorldTransform(t);
}


XMVECTOR RigidBody::GetPosition()
{
	btTransform t;
	body->getMotionState()->getWorldTransform(t);

	return Physics::VecToDX(t.getOrigin());
}


void RigidBody::SetRotation(XMVECTOR rotation)
{
	btTransform t;
	body->getMotionState()->getWorldTransform(t);
	t.setRotation(Physics::QuatFromDX(rotation));
	body->getMotionState()->setWorldTransform(t);
}


XMVECTOR RigidBody::GetRotation()
{
	btTransform t;
	body->getMotionState()->getWorldTransform(t);

	return Physics::QuatToDX(t.getRotation());
}


void RigidBody::SetLinearVelocity(XMVECTOR velocity)
{
	body->setLinearVelocity(Physics::VecFromDX(velocity));
}


XMVECTOR RigidBody::GetLinearVelocity()
{
	return Physics::VecToDX(body->getLinearVelocity());
}


void RigidBody::SetAngularVelocity(XMVECTOR velocity)
{
	body->setAngularVelocity(Physics::VecFromDX(velocity));
}


XMVECTOR RigidBody::GetAngularVelocity()
{
	return Physics::VecToDX(body->getAngularVelocity());
}


void RigidBody::SetEntity(Entity e)
{
	body->setUserIndex(e.index());
	body->setUserIndex2(e.generation());
}


Entity RigidBody::GetEntity()
{
	Entity e;
	e.id = (U64)body->getUserIndex2() << 32 | body->getUserIndex();
	return e;
}


bool RigidBody::IsDynamic()
{
	return !body->isStaticOrKinematicObject();
}


bool RigidBody::IsStatic()
{
	return body->isStaticObject();
}


bool RigidBody::IsKinematic()
{
	return body->isKinematicObject();
}