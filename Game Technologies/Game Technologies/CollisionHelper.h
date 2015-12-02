#pragma once
/**
 * Author: Callum Rhodes <c.g.rhodes@ncl.ac.uk>
 */
#include "PhysicsNode.h"

class CollisionHelper {
public:
	
	static bool SphereSphereCollision(PhysicsNode& p0, PhysicsNode& p1, CollisionData* data = NULL);
	
	static bool PlaneSphereCollision(PhysicsNode& p0, PhysicsNode& p1, CollisionData* data = NULL);

	static bool AABBCollision(PhysicsNode& p0, PhysicsNode& p1);

	static void AddCollisionImpulse(PhysicsNode& p0, PhysicsNode& p1, CollisionData& data);
};

inline float LengthSq(Vector3 v) {
	return Vector3::Dot(v, v);
}