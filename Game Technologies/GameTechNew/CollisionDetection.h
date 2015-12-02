
#pragma once
#include "TSingleton.h"
#include "PhysicsObject.h"
#include "CollisionShape.h"
#include "Manifold.h"

struct CollisionData
{
	float penetration;
	Vector3 normal;
	Vector3 pointOnPlane;
};

class CollisionDetection : public TSingleton < CollisionDetection >
{
	friend class TSingleton < CollisionDetection > ;

public:

	bool CheckCollision(PhysicsObject* obj1, PhysicsObject* obj2, CollisionShape* shape1, CollisionShape* shape2, CollisionData* out_coldata = NULL);
	void BuildCollisionManifold(float dt, PhysicsObject* obj1, PhysicsObject* obj2, CollisionShape* shape1, CollisionShape* shape2, const CollisionData& coldata, Manifold* manifold);
};