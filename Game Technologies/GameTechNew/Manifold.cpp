#include "Manifold.h"
#include "GameObject.h"
#include <nclgl\Matrix3.h>
#include <nclgl\OGLRenderer.h>

#define persistentThresholdSq 0.025f

typedef std::list<Contact> ContactList;
typedef ContactList::iterator ContactListItr;

Manifold::Manifold(PhysicsObject* nodeA, PhysicsObject* nodeB) : m_NodeA(nodeA), m_NodeB(nodeB)
{
}

Manifold::~Manifold()
{

}

void Manifold::ApplyConstraints()
{
	for (Contact& contact : m_Contacts)
	{
		contact.normal.ComputeDelta();
		contact.friction1.ComputeDelta();
		contact.friction2.ComputeDelta();

		contact.normal.Solve();

		float friction_limit = sqrtf(2.0f) * contact.normal.impulseSum;
		contact.friction1.impulseSumMin = -friction_limit;
		contact.friction1.impulseSumMax = friction_limit;
		
		contact.friction2.impulseSumMin = -friction_limit;
		contact.friction2.impulseSumMax = friction_limit;

		
		contact.friction1.Solve();
		contact.friction2.Solve();
	}
}

void Manifold::AddContact(float dt, const Vector3& globalOnA, const Vector3& globalOnB, const Vector3& normal, const float& penetration)
{
	Vector3 r1 = (globalOnA - m_NodeA->GetPosition());
	Vector3 r2 = (globalOnB - m_NodeB->GetPosition());


	Vector3 v1 = m_NodeA->GetLinearVelocity() + Vector3::Cross(m_NodeA->GetAngularVelocity(), r1);
	Vector3 v2 = m_NodeB->GetLinearVelocity() + Vector3::Cross(m_NodeB->GetAngularVelocity(), r2);
	


	Vector3 dv = v2 - v1;
	Vector3 tangent1 = dv - (normal * Vector3::Dot(dv, normal));
	if (Vector3::Dot(tangent1, tangent1) < 0.001f)
	{
		tangent1 = Vector3::Cross(normal, Vector3(1, 0, 0));
		if (Vector3::Dot(tangent1, tangent1) < 0.001f)
		{
			tangent1 = Vector3::Cross(normal, Vector3(0, 0, 1));
		}
	}
	tangent1.Normalise();

	Vector3 tangent2 = Vector3::Cross(normal, tangent1);
	tangent2.Normalise();




	//Normal Collision Constraint
	float b = 0.0f;

	//Baumgarte Offset
	{
		float baumgarte_scalar = 0.3f;
		float baumgarte_slop = 0.001f;
		float penSlop = min(penetration + baumgarte_slop, 0.0f);
		b += (baumgarte_scalar / dt) * penSlop;
	}

	//Elasticity
	{
		float elasticity = 0.8f;
		float elasticity_slop = 0.5f;

		float elatisity_term = elasticity * Vector3::Dot(normal,
			-m_NodeA->GetLinearVelocity()
			- Vector3::Cross(r1, m_NodeA->GetAngularVelocity())
			+ m_NodeB->GetLinearVelocity()
			+ Vector3::Cross(r2, m_NodeB->GetAngularVelocity())
			);

		b += min(elatisity_term + elasticity_slop, 0.0f);
	}


	//Friction Collision Constraints
	const float friction = 0.5f;



	Contact contact;
	contact.pos = globalOnA;
	contact.normal = Constraint(m_NodeA, m_NodeB,
		-normal,
		Vector3::Cross(-r1, normal),
		normal,
		Vector3::Cross(r2, normal),
		b);

	contact.normal.impulseSumMin = 0.0f;

	contact.friction1 = Constraint(m_NodeA, m_NodeB,
		-tangent1 * friction,
		Vector3::Cross(-r1, tangent1) * friction,
		tangent1 * friction,
		Vector3::Cross(r2, tangent1) * friction,
		0.0f);

	contact.friction2 = Constraint(m_NodeA, m_NodeB,
		-tangent2 * friction,
		Vector3::Cross(-r1, tangent2) * friction,
		tangent2 * friction,
		Vector3::Cross(r2, tangent2) * friction,
		0.0f);

	m_Contacts.push_back(contact);
}

void Manifold::DebugDraw()
{
	Contact& c = m_Contacts.back();

	Vector3 globalOnA = m_Contacts.back().pos;
	for (Contact& contact : m_Contacts)
	{
		Vector3 globalOnB = contact.pos;

		OGLRenderer::DrawDebugLine(DEBUGDRAW_PERSPECTIVE, globalOnA, globalOnB, Vector3(1.0f, 0.3f, 1.0f), Vector3(1.0f, 0.0f, 1.0f));
		OGLRenderer::DrawDebugCircle(DEBUGDRAW_PERSPECTIVE, globalOnA, 0.05f, Vector3(1.0f, 0.8f, 1.0f));

		globalOnA = globalOnB;
	}
}