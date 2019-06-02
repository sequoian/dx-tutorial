#pragma once

#include "RigidBody.h"
#include "Types.h"
#include <vector>
#include <btBulletDynamicsCommon.h>

class Event
{
protected:
	virtual ~Event() {};
};


class CollisionEvent : public Event
{
public:
	CollisionEvent(RigidBody a, RigidBody b, U32 numPoints, std::vector<btManifoldPoint> points) : rigidBodyA{a}, rigidBodyB{b}, numContactPoints{numPoints}, contactPoints{points} {}
	RigidBody rigidBodyA;
	RigidBody rigidBodyB;
	U32 numContactPoints;
	std::vector<btManifoldPoint> contactPoints;
};