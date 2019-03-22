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


btRigidBody* Physics::CreateRigidBody(XMVECTOR position, XMVECTOR rotation, float mass, btCollisionShape* shape)
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

	// add the body to the dynamics world
	m_dynamicsWorld->addRigidBody(body);

	return body;
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
	XMVECTOR val;
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
	XMVECTOR val;
	return val;
}