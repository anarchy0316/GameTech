#include "Manifold.h"
#include "GameObject.h"
#include <nclgl\Matrix3.h>
#include <ncltech\NCLDebug.h>
#include "PhysicsEngine.h"

#define persistentThresholdSq 0.025f

typedef std::list<Contact> ContactList;
typedef ContactList::iterator ContactListItr;

Manifold::Manifold(PhysicsObject* nodeA, PhysicsObject* nodeB) : m_NodeA(nodeA), m_NodeB(nodeB)
{
}

Manifold::~Manifold()
{

}

void Manifold::ApplyImpulse()
{
	/*
	TUTORIAL 10 CODE - Collision Resolution
	*/
}

void Manifold::PreSolverStep(float dt)
{
	/*
	TUTORIAL 10 CODE - Collision Resolution
	*/
}

void Manifold::UpdateConstraint(Contact& contact)
{
	/*
	TUTORIAL 10 CODE - Collision Resolution
	*/
}

void Manifold::AddContact(const Vector3& globalOnA, const Vector3& globalOnB, const Vector3& normal, const float& penetration)
{
	Vector3 r1 = (globalOnA - m_NodeA->GetPosition());
	Vector3 r2 = (globalOnB - m_NodeB->GetPosition());


	Contact contact;
	contact.relPosA = r1;
	contact.relPosB = r2;
	contact.collisionNormal = normal;
	contact.collisionPenetration = penetration;

	m_Contacts.push_back(contact);
}

void Manifold::DebugDraw() const
{
	if (m_Contacts.size() > 0)
	{
		const Contact& c = m_Contacts.back();

		Vector3 globalOnA1 = m_NodeA->GetPosition() +  m_Contacts.back().relPosA;
		for (const Contact& contact : m_Contacts)
		{
			Vector3 globalOnA2 = m_NodeA->GetPosition() + contact.relPosA;
			Vector3 globalOnB = m_NodeB->GetPosition() + contact.relPosB;

			NCLDebug::DrawThickLine(globalOnA1, globalOnA2, 0.02f, Vector4(0.0f, 1.0f, 0.0f, 1.0f));
			NCLDebug::DrawPoint(globalOnA2, 0.05f, Vector4(0.0f, 0.5f, 0.0f, 1.0f));

			NCLDebug::DrawThickLine(globalOnB, globalOnA2, 0.01f, Vector4(1.0f, 0.0f, 1.0f, 1.0f));

			globalOnA1 = globalOnA2;
		}
	}
}