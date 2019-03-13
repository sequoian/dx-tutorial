#include "Physics.h"


Physics::~Physics()
{
	ShutDown();
}


bool Physics::StartUp()
{
	// collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	m_collisionConfiguration = new btDefaultCollisionConfiguration();

	// use the default collision dispatcher. For parallel processing you can use a diffent dispatcher
	m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);

	// btDbvtBroadphase is a good general purpose broadphase.
	m_overlappingPairCache = new btDbvtBroadphase();

	// the default constraint solver. For parallel processing you can use a different solver
	m_solver = new btSequentialImpulseConstraintSolver;

	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_overlappingPairCache, m_solver, m_collisionConfiguration);

	// default gravity value
	SetGravity(10);

	return true;
}


void Physics::ShutDown()
{
	// remove the rigidbodies from the dynamics world and delete them
	for (int i = m_dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		m_dynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}

	// delete collision shapes
	for (int j = 0; j < m_collisionShapes.size(); j++)
	{
		btCollisionShape* shape = m_collisionShapes[j];
		m_collisionShapes[j] = 0;
		delete shape;
	}

	// delete dynamics world
	delete m_dynamicsWorld;

	// delete solver
	delete m_solver;

	// delete broadphase
	delete m_overlappingPairCache;

	// delete dispatcher
	delete m_dispatcher;

	// delete collision configuration
	delete m_collisionConfiguration;

	// next line is optional: it will be cleared by the destructor when the array goes out of scope
	m_collisionShapes.clear();
}


void Physics::RunSimulation(float deltaTime)
{
	m_dynamicsWorld->stepSimulation(deltaTime);
}


void Physics::SetGravity(float gravity)
{
	m_gravity = gravity;
	m_dynamicsWorld->setGravity(btVector3(0, -m_gravity, 0));
}


btCollisionShape* Physics::CreateCollisionBox(float x, float y, float z)
{
	btCollisionShape* shape = new btBoxShape(btVector3(btScalar(x/2), btScalar(y/2), btScalar(z/2)));
	m_collisionShapes.push_back(shape);

	return shape;
}


btRigidBody* Physics::CreateRigidBody(btVector3 position, float mass, btCollisionShape* shape)
{
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(position);

	// rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
	{
		shape->calculateLocalInertia(mass, localInertia);
	}

	// using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* motionState = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	// add the body to the dynamics world
	m_dynamicsWorld->addRigidBody(body);

	return body;
}