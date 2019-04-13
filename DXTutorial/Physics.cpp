#include "Physics.h"
#include "Assert.h"
#include "RigidBodySystem.h"


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

	// set tick callback
	m_dynamicsWorld->setInternalTickCallback(SimulationCallback);

	// allows ghost objects to be used
	m_dynamicsWorld->getPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());

	return true;
}


void Physics::ShutDown()
{
	// delete dynamics world
	if (m_dynamicsWorld)
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

		delete m_dynamicsWorld;
		m_dynamicsWorld = nullptr;
	}

	// delete collision shapes
	for (int j = 0; j < m_collisionShapes.size(); j++)
	{
		btCollisionShape* shape = m_collisionShapes[j];
		m_collisionShapes[j] = 0;
		delete shape;
		m_collisionShapes.clear();
	}
	
	// delete solver
	if (m_solver)
	{
		delete m_solver;
		m_solver = nullptr;
	}

	// delete broadphase
	if (m_overlappingPairCache)
	{
		delete m_overlappingPairCache;
		m_overlappingPairCache = nullptr;
	}

	// delete dispatcher
	if (m_dispatcher)
	{
		delete m_dispatcher;
		m_dispatcher = nullptr;
	}

	// delete collision configuration
	if (m_collisionConfiguration)
	{
		delete m_collisionConfiguration;
		m_collisionConfiguration = nullptr;
	}	
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
	btCollisionShape* shape = new btBoxShape(btVector3(btScalar(x), btScalar(y), btScalar(z)));
	m_collisionShapes.push_back(shape);

	return shape;
}

btCollisionShape* Physics::CreateCollisionSphere(float radius)
{
	btCollisionShape* shape = new btSphereShape(radius);
	m_collisionShapes.push_back(shape);

	return shape;
}


btCollisionShape* Physics::CreateCollisionCylinder(float x, float y, float z)
{
	btCollisionShape* shape = new btCylinderShape(btVector3(x, y, z));
	m_collisionShapes.push_back(shape);

	return shape;
}


btCollisionShape* Physics::CreateCollisionCone(float radius, float height)
{
	btCollisionShape* shape = new btConeShape(radius, height);
	m_collisionShapes.push_back(shape);

	return shape;
}


btRigidBody* Physics::CreateRigidBody(Entity e, XMVECTOR position, XMVECTOR rotation, float mass, btCollisionShape* shape, bool isKinematic, bool isTrigger)
{
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(VecFromDX(position));
	transform.setRotation(QuatFromDX(rotation));

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

	// alter kinematic body properties
	if (isKinematic)
	{
		ASSERT_VERBOSE(mass == 0.f, "Kinematic rigid bodies must have a mass of 0");
		body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		body->setActivationState(DISABLE_DEACTIVATION);
	}

	// alter trigger body properties
	if (isTrigger)
	{
		body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	}

	// add the body to the dynamics world
	m_dynamicsWorld->addRigidBody(body);

	// set entity reference
	body->setUserIndex(e.index());
	body->setUserIndex2(e.generation());

	return body;
}

btPairCachingGhostObject* Physics::CreateGhostObject(Entity e, XMVECTOR position, XMVECTOR rotation, btCollisionShape* shape)
{
	btPairCachingGhostObject* ghost = new btPairCachingGhostObject();

	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(VecFromDX(position));
	transform.setRotation(QuatFromDX(rotation));

	ghost->setCollisionShape(shape);
	
	// set entity reference
	ghost->setUserIndex(e.index());
	ghost->setUserIndex2(e.generation());

	m_dynamicsWorld->addCollisionObject(ghost, btBroadphaseProxy::DefaultFilter, btBroadphaseProxy::AllFilter);

	return ghost;
}


btQuaternion Physics::QuatFromDX(XMVECTOR quat)
{
	btQuaternion val;
	val.setX(quat.m128_f32[0]);
	val.setY(quat.m128_f32[1]);
	val.setZ(quat.m128_f32[2]);
	val.setW(quat.m128_f32[3]);

	return val;
}


XMVECTOR Physics::QuatToDX(btQuaternion quat)
{
	XMVECTOR val = XMVectorSet(quat.getX(), quat.getY(), quat.getZ(), quat.getW());
	return val;
}


btVector3 Physics::VecFromDX(XMVECTOR vec)
{
	btVector3 val;
	val.setX(vec.m128_f32[0]);
	val.setY(vec.m128_f32[1]);
	val.setZ(vec.m128_f32[2]);
	val.setW(vec.m128_f32[3]);
	return val;
}


XMVECTOR Physics::VecToDX(btVector3 vec)
{
	XMVECTOR val = XMVectorSet(vec.getX(), vec.getY(), vec.getZ(), 1);
	return val;
}

void SimulationCallback(btDynamicsWorld* world, btScalar timeStep)
{
	int numManifolds = world->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold = world->getDispatcher()->getManifoldByIndexInternal(i);
		const btCollisionObject* obA = contactManifold->getBody0();
		const btCollisionObject* obB = contactManifold->getBody1();

		int numContacts = contactManifold->getNumContacts();

		Entity eA, eB;
		if (numContacts > 0)
		{
			eA.id = (U64)obA->getUserIndex2() << 32 | obA->getUserIndex();
			eB.id = (U64)obB->getUserIndex2() << 32 | obB->getUserIndex();
		}

		for (int j = 0; j < numContacts; j++)
		{
			btManifoldPoint& pt = contactManifold->getContactPoint(j);
			if (pt.getDistance() < 0.f)
			{
				const btVector3& ptA = pt.getPositionWorldOnA();
				const btVector3& ptB = pt.getPositionWorldOnB();
				const btVector3& normalOnB = pt.m_normalWorldOnB;
			}
		}
	}
}