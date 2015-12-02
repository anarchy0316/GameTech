#include "PhysicsEngine.h"
#include "GameObject.h"
#include "CollisionDetection.h"

#include <nclgl\Window.h>

PhysicsEngine::PhysicsEngine()
{
	m_IsPaused			= true;
	m_UpdateTimestep	= 1.0f / 60.f;
	m_UpdateAccum		= 0.0f;//累加
	m_Gravity			=  Vector3(0.0f, -9.81f, 0.0f);
	m_DampingFactor		= 0.9999f;//阻尼
}

PhysicsEngine::~PhysicsEngine()
{
	for (PhysicsObject* obj : m_PhysicsObjects)
	{
		delete obj;
	}
	m_PhysicsObjects.clear();

	for (Constraint* c : m_Constraints)
	{
		delete c;
	}
	m_Constraints.clear();

	for (Manifold* m : m_Manifolds)
	{
		delete m;
	}
	m_Manifolds.clear();
}

void PhysicsEngine::AddPhysicsObject(PhysicsObject* obj)
{
	m_PhysicsObjects.push_back(obj);
}

void PhysicsEngine::RemovePhysicsObject(PhysicsObject* obj)
{
	auto found_loc = std::find(m_PhysicsObjects.begin(), m_PhysicsObjects.end(), obj); //auto 是自动判断变量的类型

	if (found_loc != m_PhysicsObjects.end())
	{
		m_PhysicsObjects.erase(found_loc);
	}
}

void PhysicsEngine::Update(float deltaTime)
{
	m_UpdateAccum += deltaTime;
	while (m_UpdateAccum >= m_UpdateTimestep)
	{
		m_UpdateAccum -= m_UpdateTimestep;
		if (!m_IsPaused) UpdatePhysics();
	}
}


void PhysicsEngine::UpdatePhysics()
{
	for (Manifold* m : m_Manifolds)
	{
		delete m;
	}
	m_Manifolds.clear();

	//Check for collisions
	BroadPhaseCollisions();
	NarrowPhaseCollisions();

	//Solve collision constraints
	SolveConstraints();

	//Update movement
	UpdatePhysicsObjects();
}

void PhysicsEngine::DebugRender()
{
#if DEBUGDRAW_MANIFOLDS
	for (Manifold* m : m_Manifolds)
	{
		m->DebugDraw();
	}
#endif

#if DEBUGDRAW_CONSTRAINTS
	for (Constraint* c : m_Constraints)
	{
		c->DebugDraw();
	}
#endif

#if DEBUGDRAW_COLLISIONVOLUMES
	for (PhysicsObject* obj : m_PhysicsObjects)
	{
		if (obj->GetCollisionShape() != NULL)
		{
			obj->GetCollisionShape()->DebugDraw(obj);
		}
	}
#endif
}


void PhysicsEngine::UpdatePhysicsObjects()
{
	for (PhysicsObject* obj : m_PhysicsObjects)
	{
		UpdatePhysicsObject(obj);
	}
}

void PhysicsEngine::UpdatePhysicsObject(PhysicsObject* obj)
{
	/**
	TUTORIAL2 CODE - Integrating Physics Objects
	**/


	// Semi - Implicit Euler Intergration 半隐式欧拉
	
		 obj -> m_LinearVelocity += obj -> m_Force * obj -> m_InvMass* m_UpdateTimestep;
	
		 if (obj -> m_InvMass > 0.00001f)
		
		 obj -> m_LinearVelocity += m_Gravity * m_UpdateTimestep;
	
		 // Technically this is ( m_Gravity / invMass ) * invMass * dt ,
		 // hence the check for invMass being zero is still required even
		 // though invMass cancels itself out in the gravity equation .
		
	 obj -> m_LinearVelocity = obj -> m_LinearVelocity * m_DampingFactor;
	 obj -> m_Position += obj -> m_LinearVelocity * m_UpdateTimestep;
	
		 // Angular Rotation
		
		 Vector3 angluarAccel = obj -> m_InvInertia * obj -> m_Torque;
	      obj -> m_AngularVelocity = obj -> m_AngularVelocity + angluarAccel* m_UpdateTimestep;
	
		 obj -> m_AngularVelocity = obj -> m_AngularVelocity * m_DampingFactor;
	
		 obj -> m_Orientation = obj -> m_Orientation + obj -> m_Orientation *(obj -> m_AngularVelocity * m_UpdateTimestep *0.5f);
	
		 // Quaternion equiv of the above position calculation
		
		 obj -> m_Orientation.Normalise();
	
		 obj -> m_wsTransformInvalidated = true;
	 // inform the physics object that it ’s world space transform
		 // is invalid
}

/*
VERY Simplistic broadphase!
Compiles a list of all possible collisions that need to be accurately checked, in this case
we just make a list of every object against every other object. Perhaps you can make this better
with spatial broadphase checks such as Octrees.
*/
void PhysicsEngine::BroadPhaseCollisions()
{
	m_BroadphaseCollisionPairs.clear();

	PhysicsObject *objA, *objB;

	//This is a brute force broadphase, basically compiling a list to check every object against every other object
	for (size_t i = 0; i < m_PhysicsObjects.size() - 1; ++i)
	{
		for (size_t j = i + 1; j < m_PhysicsObjects.size(); ++j)
		{
			objA = m_PhysicsObjects[i];
			objB = m_PhysicsObjects[j];

			//Check they both have collision shapes
			if (objA->GetCollisionShape() != NULL 
				&& objB->GetCollisionShape() != NULL)
			{
				CollisionPair cp;
				cp.objectA = objA;
				cp.objectB = objB;
				m_BroadphaseCollisionPairs.push_back(cp);
			}
				
		}
	}
}

void PhysicsEngine::NarrowPhaseCollisions()
{
	if (m_BroadphaseCollisionPairs.size() > 0)
	{
		CollisionData coldata;
		CollisionShape *shapeA, *shapeB;

		for (CollisionPair& cp : m_BroadphaseCollisionPairs)
		{
			shapeA = cp.objectA->GetCollisionShape();
			shapeB = cp.objectB->GetCollisionShape();

			if (CollisionDetection::Instance()->CheckCollision(cp.objectA, cp.objectB, shapeA, shapeB, &coldata))
			{
				bool okA = cp.objectA->m_OnCollisionCallback(cp.objectB);
				bool okB = cp.objectB->m_OnCollisionCallback(cp.objectA);

				if (okA && okB)
				{
					//If both objects are colliding, and both callbacks allow for default collision resolution we will build a full collision manifold
					Manifold* manifold = new Manifold(cp.objectA, cp.objectB);
					if (CollisionDetection::Instance()->BuildCollisionManifold(cp.objectA, cp.objectB, shapeA, shapeB, coldata, manifold))
					{
						m_Manifolds.push_back(manifold);
					}
					else
					{
						delete manifold;
					}
				}
			}
		}
	}
}



void PhysicsEngine::SolveConstraints()
{
	/*
	TUTORIAL 4 CODE - Distance Constraints
	TUTORIAL 9 CODE - Resolving Collision Manifolds
	*/
	for (Constraint * c : m_Constraints)
		 {
		 c->PreSolverStep(m_UpdateTimestep);
		 }
	
		 for (size_t i = 0; i < SOLVER_ITERATIONS; ++i)
		 {
		 for (Constraint * c : m_Constraints)
			 {
			 c->ApplyImpulse();
			 }
		 }
}