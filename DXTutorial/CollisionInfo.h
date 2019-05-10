#pragma once

#include <btBulletDynamicsCommon.h>
#include <vector>
#include "RigidBody.h"

class CollisionInfo
{
public:
	CollisionInfo(RigidBody a, RigidBody b, I32 dir, U32 numPoints, std::vector<btManifoldPoint> points) : self{ a }, other{ b }, direction{ dir }, numContactPoints{ numPoints }, contactPoints{ points } {}
	RigidBody self;
	RigidBody other;
	I32 direction;
	U32 numContactPoints;
	std::vector<btManifoldPoint> contactPoints;
};