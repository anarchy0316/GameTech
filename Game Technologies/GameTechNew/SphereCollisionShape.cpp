#include "SphereCollisionShape.h"
#include "PhysicsObject.h"
#include "GameObject.h"
#include <nclgl/Matrix3.h>
#include <nclgl/OGLRenderer.h>

SphereCollisionShape::SphereCollisionShape()
{
	m_Radius = 1.0f;
}

SphereCollisionShape::SphereCollisionShape(float radius)
{
	m_Radius = radius;
}

SphereCollisionShape::~SphereCollisionShape()
{

}

Matrix3 SphereCollisionShape::BuildInverseInertia(float invMass)
{
	float i = 2.5f * invMass * m_Radius * m_Radius; //SOLID
	//float i = 1.5f * invMass * m_Radius * m_Radius; //HOLLOW

	Matrix3 inertia;
	inertia._11 = i;
	inertia._22 = i;
	inertia._33 = i;

	return inertia;
}

void SphereCollisionShape::GetLocalCollisionAxes(PhysicsObject* currentObject, PhysicsObject* collidingObject, std::vector<Vector3>* out_axes)
{
	if (out_axes)
	{
		//Only one axis of collision for a sphere
		Vector3 ab = collidingObject->GetPosition() - currentObject->GetPosition();
		ab.Normalise();
		out_axes->push_back(ab);
	}
}

void SphereCollisionShape::GetMinMaxVertexOnAxis(PhysicsObject* currentObject, const Vector3& axis, Vector3* out_min, Vector3* out_max)
{
	if (out_min)
		*out_min = currentObject->GetPosition() - axis * m_Radius;

	if (out_max)
		*out_max = currentObject->GetPosition() + axis * m_Radius;
}

void SphereCollisionShape::GetIncidentReferencePolygon(PhysicsObject* currentObject, const Vector3& axis, std::list<Vector3>* out_face, Vector3* out_normal, std::vector<Plane>* out_adjacent_planes)
{
	if (out_face)
	{
		out_face->push_back(currentObject->GetPosition() + axis * m_Radius);
	}

	if (out_normal)
	{
		*out_normal = axis;
	}
}