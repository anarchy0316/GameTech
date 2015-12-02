#pragma once

#include "Constraint.h"

class DistanceConstraint : public Constraint
{
public:
	DistanceConstraint(PhysicsObject* objA, PhysicsObject* objB,
		const Vector3& globalOnA, const Vector3& globalOnB)
	{
		this->objA = objA;
		this->objB = objB;

		Vector3 ab = globalOnB - globalOnA;
		Vector3 abn = ab;
		abn.Normalise();

		Vector3 r1 = (globalOnA - objA->GetPosition());
		Vector3 r2 = (globalOnB - objB->GetPosition());

		this->j1 = -abn;
		this->j2 = Vector3::Cross(-r1, abn);
		this->j3 = abn;
		this->j4 = Vector3::Cross(r2, abn);
		this->b = 0.0f;

		delta = 0.0f;
		impulseSum = 0.0f;
		impulseSumMin = -FLT_MAX;
		impulseSumMax = FLT_MAX;


		localOnA = Matrix3::Transpose(objA->GetOrientation().ToMatrix3()) * r1;
		localOnB = Matrix3::Transpose(objB->GetOrientation().ToMatrix3()) * r2;
	}


	virtual void Update(float dt)
	{
		impulseSum = 0.0f;

		Vector3 globalOnA = objA->GetOrientation().ToMatrix3() * localOnA + objA->GetPosition();
		Vector3 globalOnB = objB->GetOrientation().ToMatrix3() * localOnB + objB->GetPosition();


		Vector3 ab = globalOnB - globalOnA;
		Vector3 abn = ab;
		abn.Normalise();

		Vector3 r1 = (globalOnA - objA->GetPosition());
		Vector3 r2 = (globalOnB - objB->GetPosition());

		this->j1 = -abn;
		this->j2 = Vector3::Cross(-r1, abn);
		this->j3 = abn;
		this->j4 = Vector3::Cross(r2, abn);
	}


	virtual void DebugDraw()
	{
		Vector3 globalOnA = objA->GetOrientation().ToMatrix3() * localOnA + objA->GetPosition();
		Vector3 globalOnB = objB->GetOrientation().ToMatrix3() * localOnB + objB->GetPosition();

		OGLRenderer::DrawDebugLine(DEBUGDRAW_PERSPECTIVE, globalOnA, globalOnB, Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f));

		OGLRenderer::DrawDebugCircle(DEBUGDRAW_PERSPECTIVE, globalOnA, 0.05f, Vector3(1.0f, 0.8f, 1.0f));
		OGLRenderer::DrawDebugCircle(DEBUGDRAW_PERSPECTIVE, globalOnB, 0.05f, Vector3(1.0f, 0.8f, 1.0f));
	}

protected:
	Vector3 localOnA, localOnB;
};