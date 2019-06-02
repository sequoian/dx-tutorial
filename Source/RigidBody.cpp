#include "RigidBody.h"
#include "Physics.h"


RigidBody::RigidBody(btRigidBody* body)
{
	m_body = body;
}

RigidBody::~RigidBody()
{
}


void RigidBody::SetTransform(XMMATRIX transform)
{
	m_body->getMotionState()->setWorldTransform(Physics::MatFromDX(transform));
}


void RigidBody::SetTransform(XMVECTOR position, XMVECTOR rotation)
{
	btTransform t(Physics::QuatFromDX(rotation), Physics::VecFromDX(position));
	m_body->getMotionState()->setWorldTransform(t);
}


XMMATRIX RigidBody::GetTransform()
{
	btTransform t;
	m_body->getMotionState()->getWorldTransform(t);

	return Physics::MatToDX(t);
}


void RigidBody::SetPosition(XMVECTOR position)
{
	btTransform t;
	m_body->getMotionState()->getWorldTransform(t);
	t.setOrigin(Physics::VecFromDX(position));
	m_body->getMotionState()->setWorldTransform(t);
}


XMVECTOR RigidBody::GetPosition()
{
	btTransform t;
	m_body->getMotionState()->getWorldTransform(t);

	return Physics::VecToDX(t.getOrigin());
}


void RigidBody::SetRotation(XMVECTOR rotation)
{
	btTransform t;
	m_body->getMotionState()->getWorldTransform(t);
	t.setRotation(Physics::QuatFromDX(rotation));
	m_body->getMotionState()->setWorldTransform(t);
}


XMVECTOR RigidBody::GetRotation()
{
	btTransform t;
	m_body->getMotionState()->getWorldTransform(t);

	return Physics::QuatToDX(t.getRotation());
}


void RigidBody::SetLinearVelocity(XMVECTOR velocity)
{
	m_body->setLinearVelocity(Physics::VecFromDX(velocity));
}


XMVECTOR RigidBody::GetLinearVelocity()
{
	return Physics::VecToDX(m_body->getLinearVelocity());
}


void RigidBody::SetAngularVelocity(XMVECTOR velocity)
{
	m_body->setAngularVelocity(Physics::VecFromDX(velocity));
}


XMVECTOR RigidBody::GetAngularVelocity()
{
	return Physics::VecToDX(m_body->getAngularVelocity());
}


void RigidBody::SetEntity(Entity e)
{
	m_body->setUserIndex(e.index());
	m_body->setUserIndex2(e.generation());
}


Entity RigidBody::GetEntity()
{
	Entity e;
	e.id = (U64)m_body->getUserIndex2() << 32 | m_body->getUserIndex();
	return e;
}


bool RigidBody::IsDynamic()
{
	return !m_body->isStaticOrKinematicObject();
}


bool RigidBody::IsStatic()
{
	return m_body->isStaticObject();
}


bool RigidBody::IsKinematic()
{
	return m_body->isKinematicObject();
}


void RigidBody::SetGravity(float gravity)
{
	btVector3 grav(0, gravity, 0);
	m_body->setGravity(grav);
}


bool RigidBody::IsTrigger()
{
	return m_body->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE ? true : false;
}