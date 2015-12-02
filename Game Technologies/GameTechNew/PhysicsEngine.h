
#pragma once
#include "TSingleton.h"
#include "PhysicsObject.h"
#include "Manifold.h"
#include <vector>

#define USE_MULTITHREADING	 0

class PhysicsEngine : public TSingleton<PhysicsEngine>
{
	friend class TSingleton < PhysicsEngine > ;
public:
	void AddPhysicsObject(PhysicsObject* obj);
	void RemovePhysicsObject(PhysicsObject* obj);

	void Update(float deltaTime);			//Remember DeltaTime is 'seconds' since last update not milliseconds

	void AddConstraint(Constraint* c) { m_Constraints.push_back(c); }

protected:
	PhysicsEngine();

	void UpdatePhysicsObjectsST(float dt);
	void UpdatePhysicsObjectsMT(float dt);
	void UpdatePhysicsObject(float dt, PhysicsObject* obj);

	
	void CheckCollisions(float dt);

protected:
	Vector3		m_Gravity;
	float		m_DampingFactor;

	std::vector<PhysicsObject*> m_PhysicsObjects;

	std::vector<Constraint*> m_Constraints;		// Misc constraints between pairs of objects
	std::vector<Manifold> m_Manifolds;			// Contact constraints between pairs of objects
};