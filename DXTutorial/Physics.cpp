#include "Physics.h"
#include "Assert.h"

Physics::~Physics()
{
	ShutDown();
}


bool Physics::StartUp(EventBus* eventBus)
{
	m_eventBus = eventBus;

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
	SimulationCallback(m_dynamicsWorld, deltaTime);
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


RigidBody Physics::CreateDynamicRigidBody(Entity e, btCollisionShape* shape, XMVECTOR position, XMVECTOR rotation, float mass)
{
	ASSERT_VERBOSE(mass > 0.f, "Dynamic rigid bodies must have a mass greater than 0");
	if (mass < 0.f)
	{
		mass = 1.f;
	}

	btTransform transform;
	transform.setIdentity();
	transform.setRotation(QuatFromDX(rotation));
	transform.setOrigin(VecFromDX(position));

	btVector3 localInertia(0, 0, 0);
	shape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* motionState = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);
	
	m_dynamicsWorld->addRigidBody(body);

	RigidBody rb(body);
	rb.SetEntity(e);
	return rb;
}


RigidBody Physics::CreateStaticRigidBody(Entity e, btCollisionShape* shape, XMVECTOR position, XMVECTOR rotation, bool isTrigger)
{
	btTransform transform;
	transform.setIdentity();
	transform.setRotation(QuatFromDX(rotation));
	transform.setOrigin(VecFromDX(position));
	btVector3 localInertia(0, 0, 0);

	btDefaultMotionState* motionState = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(0, motionState, shape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	if (isTrigger)
	{
		body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	}

	m_dynamicsWorld->addRigidBody(body);

	RigidBody rb(body);
	rb.SetEntity(e);
	return rb;
}


RigidBody Physics::CreateKinematicRigidBody(Entity e, btCollisionShape* shape, XMVECTOR position, XMVECTOR rotation, bool isTrigger)
{
	btTransform transform;
	transform.setIdentity();
	transform.setRotation(QuatFromDX(rotation));
	transform.setOrigin(VecFromDX(position));
	btVector3 localInertia(0, 0, 0);

	btDefaultMotionState* motionState = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(0, motionState, shape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	// kinematic flags
	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	body->setActivationState(DISABLE_DEACTIVATION);

	if (isTrigger)
	{
		body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	}

	m_dynamicsWorld->addRigidBody(body);

	RigidBody rb(body);
	rb.SetEntity(e);
	return rb;
}


RigidBody Physics::CreateCharacterBody(Entity e, btCollisionShape* shape, XMVECTOR position, XMVECTOR rotation)
{
	btTransform transform;
	transform.setIdentity();
	transform.setRotation(QuatFromDX(rotation));
	transform.setOrigin(VecFromDX(position));
	btVector3 localInertia(0, 0, 0);

	btDefaultMotionState* motionState = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(0, motionState, shape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	// kinematic flags
	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	body->setActivationState(DISABLE_DEACTIVATION);

	m_dynamicsWorld->addRigidBody(body, btBroadphaseProxy::DefaultFilter, btBroadphaseProxy::AllFilter);

	RigidBody rb(body);
	rb.SetEntity(e);
	return rb;
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


btTransform Physics::MatFromDX(XMMATRIX mat)
{
	btVector3 pos = Physics::VecFromDX(mat.r[3]);
	btQuaternion rot = Physics::QuatFromDX(XMQuaternionRotationMatrix(mat));
	btTransform t(rot, pos);

	return t;
}


XMMATRIX Physics::MatToDX(btTransform mat)
{
	float m[16];
	mat.getOpenGLMatrix(m);
	return XMMATRIX(m);
}


void Physics::SimulationCallback(btDynamicsWorld* world, btScalar timeStep)
{
	int numManifolds = world->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold = world->getDispatcher()->getManifoldByIndexInternal(i);

		// Get first rigid body
		const btCollisionObject* obA = contactManifold->getBody0();
		const btRigidBody* crbA = static_cast<const btRigidBody*>(obA);
		btRigidBody* rbA = const_cast<btRigidBody*>(crbA);
		RigidBody rigidBodyA = RigidBody(rbA);

		// Get second rigid body
		const btCollisionObject* obB = contactManifold->getBody1();
		const btRigidBody* crbB = static_cast<const btRigidBody*>(obB);
		btRigidBody* rbB = const_cast<btRigidBody*>(crbB);
		RigidBody rigidBodyB = RigidBody(rbB);

		int numContacts = contactManifold->getNumContacts();
		if (numContacts <= 0)
		{
			return;
		}

		std::vector<btManifoldPoint> points;
		for (int j = 0; j < numContacts; j++)
		{
			btManifoldPoint& pt = contactManifold->getContactPoint(j);
			if (pt.getDistance() < 0.f)
			{
				points.push_back(pt);
			}
		}

		m_eventBus->Publish(&CollisionEvent(rigidBodyA, rigidBodyB, numContacts, points));
	}
}