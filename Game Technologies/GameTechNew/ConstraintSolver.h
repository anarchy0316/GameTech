
#pragma once

#include "TSingleton.h"
#include "PhysicsObject.h"
#include <nclgl\Vector3.h>
#include <vector>

struct Constraint
{
	Constraint() {}

	PhysicsObject *objA, *objB;

	Vector3 j1, j2, j3, j4;
	float b;

	float impulseSum;
	float impulseSumClampMax;
	float impulseSumClampMin;

	float delta;
};

class ConstraintSolver : public TSingleton < ConstraintSolver >
{
	friend class TSingleton < ConstraintSolver >;

public:
	Constraint* GetNewConstraint(int threadIdx);

	void ClearConstraints();
	void SolveConstraints(float dt);

protected:

protected:
	std::vector<Constraint> m_Constraints;
};