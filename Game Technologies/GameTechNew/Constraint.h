
#pragma once
#include "PhysicsObject.h"
#include <nclgl\Vector3.h>

class Constraint
{
public:
	Constraint();

	Constraint(PhysicsObject* objA, PhysicsObject* objB,
		Vector3 j1, Vector3 j2, Vector3 j3, Vector3 j4, float b);



	void ComputeDelta();
	void Solve();

	virtual void Update(float dt) {}
	virtual void DebugDraw() {}

public:
	PhysicsObject *objA, *objB;

	Vector3 j1, j2, j3, j4;
	float b;

	float delta;

	float impulseSum;
	float impulseSumMin, impulseSumMax;
};