#pragma once

#include "btBulletDynamicsCommon.h"
#include "WriteLog.h"

class Physics
{
public:
	~Physics();
	bool StartUp();
	void ShutDown();
	void RunSimulation(float deltaTime);
	void SetGravity(float gravity);
	btCollisionShape* CreateCollisionBox(float x, float y, float z);
	btRigidBody* CreateRigidBody(btVector3 position, float mass, btCollisionShape* shape);

private:
	btDefaultCollisionConfiguration* m_collisionConfiguration;
	btCollisionDispatcher* m_dispatcher;
	btBroadphaseInterface* m_overlappingPairCache;
	btSequentialImpulseConstraintSolver* m_solver;
	btDiscreteDynamicsWorld* m_dynamicsWorld;
	btAlignedObjectArray<btCollisionShape*> m_collisionShapes;
	float m_gravity;
};