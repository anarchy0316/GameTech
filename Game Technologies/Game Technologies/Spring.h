#pragma once
/**
 * Author: Callum Rhodes <c.g.rhodes@ncl.ac.uk>
 */
#include "Constraint.h"
#include "DebugDrawer.h"
#include "PhysicsNode.h"

class Spring : public Constraint, public DebugDrawer {
public:
	Spring(PhysicsNode* p0, Vector3 localPos0, PhysicsNode* p1, Vector3 localPos1):
		p0(p0), localPos0(localPos0), p1(p1), localPos1(localPos1) {
		ks = 0.0001f; // defaults
		kd = 0.01f;
		
		Vector3 pos0 = p0->BuildTransform()*localPos0;
		Vector3 pos1 = p1->BuildTransform()*localPos1;

		length = (pos0 - pos1).Length();
	}

	void Update(float msec);

	void DebugDraw();

private:
	PhysicsNode* p0;
	PhysicsNode* p1;
	Vector3 localPos0;
	Vector3 localPos1;
	float length; // rest length
	float ks;	  // stiffness
	float kd;	  // damping
};

