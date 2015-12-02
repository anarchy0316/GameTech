#include "PhysicsEngine.h"
#include "TaskScheduler.h"
#include "GameObject.h"
#include "CollisionDetection.h"

#include <nclgl\Window.h>

PhysicsEngine::PhysicsEngine()
{
	m_Gravity =  Vector3(0.0f, -9.81f, 0.0f);
	m_DampingFactor = 1.0f;// 0.999f;
}

void PhysicsEngine::AddPhysicsObject(PhysicsObject* obj)
{
	m_PhysicsObjects.push_back(obj);
}

void PhysicsEngine::RemovePhysicsObject(PhysicsObject* obj)
{
	auto found_loc = std::find(m_PhysicsObjects.begin(), m_PhysicsObjects.end(), obj);

	if (found_loc != m_PhysicsObjects.end())
	{
		m_PhysicsObjects.erase(found_loc);
	}
}

bool paused = true;
void PhysicsEngine::Update(float deltaTime)
{
	m_Manifolds.clear();

	//Check for collisions
	CheckCollisions(deltaTime);


	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_P))
		paused = !paused;

	if (!paused)
	{
		for (int i = 0; i < 20; ++i)
		{
			for (Manifold& m : m_Manifolds)
			{
				m.ApplyConstraints();
			}

			for (Constraint* c : m_Constraints)
			{
				c->Update(deltaTime);
				c->ComputeDelta();
				c->Solve();
			}
		}

		//Update movement
#if USE_MULTITHREADING
		UpdatePhysicsObjectsMT(deltaTime);
#else
		UpdatePhysicsObjectsST(deltaTime);
#endif
	}


	//If Debug
	for (Manifold& m : m_Manifolds)
	{
		m.DebugDraw();
	}
	for (Constraint* c : m_Constraints)
	{
		c->DebugDraw();
	}
}


void PhysicsEngine::UpdatePhysicsObjectsST(float dt)
{
	for (auto obj : m_PhysicsObjects)
		UpdatePhysicsObject(dt, obj);
}

void PhysicsEngine::UpdatePhysicsObjectsMT(float dt)
{
	if (m_PhysicsObjects.size() > 0)
	{
		int queue_idx = TaskScheduler::Instance()->BeginNewTaskQueue();

		for (auto obj : m_PhysicsObjects)
			TaskScheduler::Instance()->PostTaskToQueue(queue_idx, std::bind(&PhysicsEngine::UpdatePhysicsObject, this, dt, obj));

		TaskScheduler::Instance()->WaitForTaskQueueToComplete(queue_idx);
	}
}

void PhysicsEngine::UpdatePhysicsObject(float dt, PhysicsObject* obj)
{
	//Semi-Implicit Euler Intergration
	obj->m_LinearVelocity += obj->m_Force * obj->m_InvMass * dt;

	if (obj->m_InvMass > 0.00001f)
		obj->m_LinearVelocity += m_Gravity * dt; //Technically this is (m_Gravity / invMass) * invMass * dt, hence the check for invMass being zero is still required

	obj->m_LinearVelocity = obj->m_LinearVelocity * m_DampingFactor;
	obj->m_Position += obj->m_LinearVelocity * dt;


	//Angular Rotation
	Vector3 angluarAccel = obj->m_InvInertia * obj->m_Torque;
	obj->m_AngularVelocity = obj->m_AngularVelocity + angluarAccel * dt;
	obj->m_AngularVelocity = obj->m_AngularVelocity * m_DampingFactor;

	obj->m_Orientation = obj->m_Orientation + obj->m_Orientation*(obj->m_AngularVelocity*dt*0.5f); //Quaternion equiv of the above position calculation
	obj->m_Orientation.Normalise();

	obj->m_wsTransformInvalidated = true; //inform the physics object that it's world space transform is invalid
}

void PhysicsEngine::CheckCollisions(float dt)
{
	CollisionData coldata;
	CollisionShape *shapeA, *shapeB;

	PhysicsObject *objA, *objB;
	for (int i = 0; i < m_PhysicsObjects.size() - 1; ++i)
	{
		for (int j = i + 1; j < m_PhysicsObjects.size(); ++j)
		{
			objA = m_PhysicsObjects[i];
			objB = m_PhysicsObjects[j];

			//Check they both have collision shapes
			shapeA = objA->GetCollisionShape();
			shapeB = objB->GetCollisionShape();

			if (shapeA != NULL && shapeB != NULL)
			{
				if (CollisionDetection::Instance()->CheckCollision(objA, objB, shapeA, shapeB, &coldata))
				{
					m_Manifolds.push_back(Manifold(objA, objB));
					CollisionDetection::Instance()->BuildCollisionManifold(dt, objA, objB, shapeA, shapeB, coldata, &m_Manifolds.back());
				}
			}
		}
	}
}