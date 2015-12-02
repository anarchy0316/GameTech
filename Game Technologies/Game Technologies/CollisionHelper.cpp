#include "CollisionHelper.h"

bool CollisionHelper::SphereSphereCollision(PhysicsNode& p0, PhysicsNode& p1, CollisionData* data) {
	CollisionSphere& s0 = *(CollisionSphere*)p0.GetCollisionVolume();
	CollisionSphere& s1 = *(CollisionSphere*)p1.GetCollisionVolume();

	Vector3 normal = p0.GetPosition() - p1.GetPosition();
	const float distSq = LengthSq(normal);
	const float sumRadius = s0.GetRadius() + s1.GetRadius();

	if (distSq < sumRadius*sumRadius) {
		if (data) {
			data->m_penetration = sumRadius - sqrtf(distSq);
			normal.Normalise();
			data->m_normal = normal;
			data->m_point = p0.GetPosition() - normal*(s0.GetRadius() - data->m_penetration*0.5f);
		}
		return true;
	}
	return false;
}

bool CollisionHelper::PlaneSphereCollision(PhysicsNode& p0, PhysicsNode& p1, CollisionData* data) {
	CollisionPlane& plane = *(CollisionPlane*)p0.GetCollisionVolume();
	CollisionSphere& sphere = *(CollisionSphere*)p1.GetCollisionVolume();

	float separation = Vector3::Dot(p1.GetPosition(), plane.GetNormal()) - plane.GetDistance();

	if (separation > sphere.GetRadius()) {
		return false;
	}
	if (data) {
		data->m_penetration = sphere.GetRadius() - separation;
		data->m_normal = -plane.GetNormal();
		data->m_point = p1.GetPosition() - plane.GetNormal()*separation;
	}

	return true;
}


bool CollisionHelper::AABBCollision(PhysicsNode& p0, PhysicsNode& p1) {
	CollisionAABB& aabb0 = *(CollisionAABB*)p0.GetCollisionVolume();
	CollisionAABB& aabb1 = *(CollisionAABB*)p1.GetCollisionVolume();
	
	float dist = abs(p0.GetPosition().x - p1.GetPosition().x);
	float sum = aabb0.getHalfDimensions().x + aabb1.getHalfDimensions().x;
	if (dist <= sum) {
		dist = abs(p0.GetPosition().y - p1.GetPosition().y);
		sum = aabb0.getHalfDimensions().y + aabb1.getHalfDimensions().y;
		if (dist <= sum) {
			dist = abs(p0.GetPosition().z - p1.GetPosition().z);
			sum = aabb0.getHalfDimensions().z + aabb1.getHalfDimensions().z;
			if (dist <= sum) {
				return true;
			}
		}
	}
	return false;
}

void CollisionHelper::AddCollisionImpulse(PhysicsNode& p0, PhysicsNode& p1, CollisionData& data) {
	
	if (p0.GetInverseMass() + p1.GetInverseMass() == 0.0f) return;
	
	Vector3 r0 = data.m_point - p0.GetPosition();
	Vector3 r1 = data.m_point - p1.GetPosition();
	
	Vector3 v0 = p0.GetLinearVelocity() + Vector3::Cross(p0.GetAngularVelocity(), r0);
	Vector3 v1 = p1.GetLinearVelocity() + Vector3::Cross(p1.GetAngularVelocity(), r1);

	Vector3 dv = v0 - v1;

	float relMov = -Vector3::Dot(dv, data.m_normal);
	if (relMov < -0.01f) return;

	{
		float e = 0.0f;
		float normDiv = (p0.GetInverseMass() + p1.GetInverseMass()) +
			Vector3::Dot(data.m_normal,
				Vector3::Cross(p0.GetInverseInertia()*Vector3::Cross(r0, data.m_normal), r0) +
				Vector3::Cross(p1.GetInverseInertia()*Vector3::Cross(r1, data.m_normal), r1));
		float jn = -1*(1+e)*Vector3::Dot(dv, data.m_normal)/normDiv;

		jn = jn + (data.m_penetration*0.01f);

		Vector3 l0 = p0.GetLinearVelocity() + data.m_normal*(jn*p0.GetInverseMass());
		p0.SetLinearVelocity(l0);
		Vector3 a0 = p0.GetAngularVelocity() + p0.GetInverseInertia()* Vector3::Cross(r0, data.m_normal * jn);
		p0.SetAngularVelocity(a0);

		Vector3 l1 = p1.GetLinearVelocity() - data.m_normal*(jn*p1.GetInverseMass());
		p1.SetLinearVelocity(l1);
		Vector3 a1 = p1.GetAngularVelocity() - p1.GetInverseInertia()* Vector3::Cross(r1, data.m_normal * jn);
		p1.SetAngularVelocity(a1);

	}

	{
		Vector3 tangent = dv - data.m_normal*Vector3::Dot(dv, data.m_normal);
		tangent.Normalise();
		float tangDiv = (p0.GetInverseMass() + p1.GetInverseMass()) +
			Vector3::Dot(tangent,
			Vector3::Cross( p0.GetInverseInertia()* Vector3::Cross(r0, tangent), r0) +
			Vector3::Cross( p1.GetInverseInertia()* Vector3::Cross(r1, tangent), r1));

		float jt = -1* Vector3::Dot(dv, tangent) / tangDiv;

		Vector3 l0 = p0.GetLinearVelocity() + tangent*(jt*p0.GetInverseMass());
		p0.SetLinearVelocity(l0);
		Vector3 a0 = p0.GetAngularVelocity() + p0.GetInverseInertia()* Vector3::Cross(r0, tangent * jt);
		p0.SetAngularVelocity(a0);

		Vector3 l1 = p1.GetLinearVelocity() - tangent*(jt*p1.GetInverseMass());
		p1.SetLinearVelocity(l1);
		Vector3 a1 = p1.GetAngularVelocity() - p1.GetInverseInertia()* Vector3::Cross(r1, tangent * jt);
		p1.SetAngularVelocity(a1);
	}

}