
#pragma once

#include "CollisionShape.h"
#include "Hull.h"

class CuboidCollisionShape : public CollisionShape
{
public:
	CuboidCollisionShape();
	CuboidCollisionShape(float width, float height, float depth);
	~CuboidCollisionShape();

	virtual Matrix3 BuildInverseInertia(float invMass) override;

	virtual void GetLocalCollisionAxes(PhysicsObject* currentObject, PhysicsObject* collidingObject, std::vector<Vector3>* out_axes) override;
	virtual void GetMinMaxVertexOnAxis(PhysicsObject* currentObject, const Vector3& axis, Vector3* out_min, Vector3* out_max) override;
	virtual void GetIncidentReferencePolygon(PhysicsObject* currentObject, const Vector3& axis, std::list<Vector3>* out_face, Vector3* out_normal, std::vector<Plane>* out_adjacent_planes) override;

	void SetWidth(float width)		{ m_CuboidDimensions.x = fabs(width); m_CuboidHalfDimensions.x = m_CuboidDimensions.x * 0.5f; }
	void SetHeight(float height)	{ m_CuboidDimensions.x = fabs(height); m_CuboidHalfDimensions.y = m_CuboidDimensions.y * 0.5f; }
	void SetDepth(float depth)		{ m_CuboidDimensions.x = fabs(depth); m_CuboidHalfDimensions.z = m_CuboidDimensions.z * 0.5f; }

	float GetWidth()	{ return m_CuboidDimensions.x; }
	float GetHeight()	{ return m_CuboidDimensions.y; }
	float GetDepth()	{ return m_CuboidDimensions.z; }




protected:
	virtual void OnLocalTransformUpdated() override;


	static void ConstructCubeHull();

protected:
	Vector3				 m_CuboidDimensions;
	Vector3				 m_CuboidHalfDimensions;
	std::vector<Vector3> m_CuboidAxes;			//These are static and can therefore be pre-computed

	static Hull			 m_CubeHull;
}; 

