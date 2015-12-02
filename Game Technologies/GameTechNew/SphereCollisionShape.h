
#pragma once

#include "CollisionShape.h"

class SphereCollisionShape : public CollisionShape
{
public:
	SphereCollisionShape();
	SphereCollisionShape(float radius);
	~SphereCollisionShape();

	virtual Matrix3 BuildInverseInertia(float invMass) override;

	virtual void GetLocalCollisionAxes(PhysicsObject* currentObject, PhysicsObject* collidingObject, std::vector<Vector3>* out_axes) override;
	virtual void GetMinMaxVertexOnAxis(PhysicsObject* currentObject, const Vector3& axis, Vector3* out_min, Vector3* out_max) override;
	virtual void GetIncidentReferencePolygon(PhysicsObject* currentObject, const Vector3& axis, std::list<Vector3>* out_face, Vector3* out_normal, std::vector<Plane>* out_adjacent_planes) override;

	void SetRadius(float radius)		{ m_Radius = radius; }
	float GetRadius()					{ return m_Radius; }

protected:
	float				 m_Radius;
};

