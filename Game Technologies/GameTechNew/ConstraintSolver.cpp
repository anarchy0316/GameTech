#include "ConstraintSolver.h"

#define SOLVER_ITERATIONS 20

Constraint* ConstraintSolver::GetNewConstraint(int threadIdx)
{
	m_Constraints.push_back(Constraint());

	return &m_Constraints.back();
}

void ConstraintSolver::SolveConstraints(float dt)
{
	for (int i = 0; i < SOLVER_ITERATIONS; ++i)
	{
		//Compute Deltas all at once (before updates to vel/rot occur)
		for (Constraint& c : m_Constraints)
		{
			//J * M(-1) * J(t)
			float contstraint_mass = c.objA->GetInverseMass() * Vector3::Dot(c.j1, c.j1)
				+ Vector3::Dot(c.j2, (c.objA->GetInverseInertia() * c.j2))
				+ c.objB->GetInverseMass() * Vector3::Dot(c.j3, c.j3)
				+ Vector3::Dot(c.j4, (c.objB->GetInverseInertia() * c.j4));

			//JV
			float jv = Vector3::Dot(c.j1, c.objA->GetLinearVelocity())
				+ Vector3::Dot(c.j2, c.objA->GetAngularVelocity())
				+ Vector3::Dot(c.j3, c.objB->GetLinearVelocity())
				+ Vector3::Dot(c.j4, c.objB->GetAngularVelocity());

			float denom = -(jv + c.b);
			c.delta = (fabs(contstraint_mass) > 0.00001f) ? (denom / contstraint_mass) : 0.0f;
		}


		//Apply deltas to objects simulatenously
		for (Constraint& c : m_Constraints)
		{
			//Clamp total applied impulse to range (e.g to make a distance constraint only apply a force in the outward direction)
			float oldImpulseSum = c.impulseSum;
			c.impulseSum = max(c.impulseSum + c.delta, c.impulseSumClampMin);
			c.impulseSum = min(c.impulseSum, c.impulseSumClampMax);

			float realDelta = c.impulseSum - oldImpulseSum;

			//Apply updates
			c.objA->SetLinearVelocity(c.objA->GetLinearVelocity() + (c.j1 * realDelta) * c.objA->GetInverseMass());
			c.objA->SetAngularVelocity(c.objA->GetAngularVelocity() + c.objA->GetInverseInertia() * (c.j2 * realDelta));
			c.objB->SetLinearVelocity(c.objB->GetLinearVelocity() + (c.j3 * realDelta) * c.objB->GetInverseMass());
			c.objB->SetAngularVelocity(c.objB->GetAngularVelocity() + c.objB->GetInverseInertia() * (c.j4 * realDelta));
		}
	}
}
