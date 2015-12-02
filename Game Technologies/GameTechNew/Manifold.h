#pragma once

#include "PhysicsObject.h"
#include <list>
#include <nclgl\Vector3.h>
#include "Constraint.h"

struct Contact
{
	Constraint normal;
	Constraint friction1;
	Constraint friction2;

	//Debug Drawing
	Vector3 pos;
};

class Manifold
{
public:
	Manifold(PhysicsObject* nodeA, PhysicsObject* nodeB);
	~Manifold();

	void AddContact(float dt, const Vector3& globalOnA, const Vector3& globalOnB, const Vector3& normal, const float& penetration);	//Called whenever a new collision contact between A & B are found

	void ApplyConstraints();

	void DebugDraw();

	PhysicsObject* NodeA() { return m_NodeA; }
	PhysicsObject* NodeB() { return m_NodeB; }
protected:
	PhysicsObject*			m_NodeA;
	PhysicsObject*			m_NodeB;
	std::vector<Contact>	m_Contacts;
};