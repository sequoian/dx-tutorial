#pragma once

#include "Event.h"

class CollisionEvent : public Event
{
public:
	CollisionEvent(RigidBody a, RigidBody b, U32 numPoints, std::vector<btManifoldPoint> points) : rigidBodyA{ a }, rigidBodyB{ b }, numContactPoints{ numPoints }, contactPoints{ points } {}
	RigidBody rigidBodyA;
	RigidBody rigidBodyB;
	U32 numContactPoints;
	std::vector<btManifoldPoint> contactPoints;
};