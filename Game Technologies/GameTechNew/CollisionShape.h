
#pragma once

#include "Hull.h"

#include <nclgl/Vector3.h>
#include <nclgl\Matrix4.h>
#include <nclgl\Quaternion.h>
#include <nclgl\Plane.h>
#include <vector>
#include <list>

struct Edge
{
	Edge() :
		start(), end() {}

	Edge(const Vector3& p0, const Vector3& p1) :
		start(p0), end(p1) {}

	Vector3 start;
	Vector3 end;
};

class PhysicsObject;

class CollisionShape
{
public:
	CollisionShape()	{}
	~CollisionShape()	{}

	virtual Matrix3 BuildInverseInertia(float invMass) = 0;

	virtual void GetLocalCollisionAxes(PhysicsObject* currentObject, PhysicsObject* collidingObject, std::vector<Vector3>* out_axes) = 0;
	virtual void GetMinMaxVertexOnAxis(PhysicsObject* currentObject, const Vector3& axis, Vector3* out_min, Vector3* out_max)		= 0;
	virtual void GetIncidentReferencePolygon(PhysicsObject* currentObject, const Vector3& axis, std::list<Vector3>* out_face, Vector3* out_normal, std::vector<Plane>* out_adjacent_planes) = 0;

	//Only allow the position and orientation of the collision shape to be set, all scaling should be handled by the collision shape implementation
	const Vector3& GetRelativePosition()	{ return m_LocalTransform.GetPositionVector(); }
	const Quaternion& GetRelativeOrientation() { return Quaternion::FromMatrix(m_LocalTransform); }

	void SetRelativePosition(const Vector3& relPos) { 
		m_LocalTransform.SetPositionVector(relPos);
	}

	void SetRelativeOrientation(const Quaternion& relRotation) {
		Vector3 pos = GetRelativePosition();
		m_LocalTransform = relRotation.ToMatrix4();
		m_LocalTransform.SetPositionVector(pos);
	}

protected:
	virtual void OnLocalTransformUpdated() {};

protected:
	Matrix4 m_LocalTransform;	//The relative rotation and position of this collision shape relative to it's parent object
};