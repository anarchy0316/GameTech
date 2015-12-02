

#pragma once

#include "Constraint.h"
#include "NCLDebug.h"
#include "PhysicsEngine.h"

class DistanceConstraint : public Constraint
{
public:
	DistanceConstraint(PhysicsObject* objA, PhysicsObject* objB,
		const Vector3& globalOnA, const Vector3& globalOnB)
	{
		this->objA = objA;
		this->objB = objB;

		Vector3 ab = globalOnB - globalOnA;
		this->distance = ab.Length();

		Vector3 r1 = (globalOnA - objA->GetPosition());
		Vector3 r2 = (globalOnB - objB->GetPosition());
		localOnA = Matrix3::Transpose(objA->GetOrientation().ToMatrix3()) * r1;
		localOnB = Matrix3::Transpose(objB->GetOrientation().ToMatrix3()) * r2;

		delta = 0.0f;
		impulseSum = 0.0f;
		impulseSumMin = -FLT_MAX;
		impulseSumMax = FLT_MAX;
	}


	virtual void PreSolverStep(float dt)
	{
		/*
		TUTORIAL4 CODE - Constraints
		*/
		Vector3 r1 = objA->GetOrientation().ToMatrix3() * localOnA;
		 Vector3 r2 = objB->GetOrientation().ToMatrix3() * localOnB;
		
			 Vector3 globalOnA = r1 + objA->GetPosition();
		 Vector3 globalOnB = r2 + objB->GetPosition();
		
			 Vector3 ab = globalOnB - globalOnA;
		 Vector3 abn = ab;
		 abn.Normalise();
		
			 this->j1 = -abn;
		 this->j2 = Vector3::Cross(-r1, abn);
		 this->j3 = abn;
		 this->j4 = Vector3::Cross(r2, abn);
		
			 // Baumgarte Offset ( Adds energy to the system to counter slight
			 // solving errors that accumulate over time - known as
			 // 'constraint drift ')
			
			 {
			 float distance_offset = distance - ab.Length();
			 float baumgarte_scalar = 0.1f;
			 b = -(baumgarte_scalar / PhysicsEngine::Instance()->
				 GetDeltaTime()) * distance_offset;
			 }
	}

	virtual void DebugDraw() const
	{
		Vector3 globalOnA = objA->GetOrientation().ToMatrix3() * localOnA + objA->GetPosition();
		Vector3 globalOnB = objB->GetOrientation().ToMatrix3() * localOnB + objB->GetPosition();

		NCLDebug::DrawThickLine(globalOnA, globalOnB, 0.02f, Vector4(0.0f, 0.0f, 0.0f, 1.0f));
		NCLDebug::DrawPoint(globalOnA, 0.05f, Vector4(1.0f, 0.8f, 1.0f, 1.0f));
		NCLDebug::DrawPoint(globalOnB, 0.05f, Vector4(1.0f, 0.8f, 1.0f, 1.0f));
	}

protected:
	float   distance;
	Vector3 localOnA, localOnB;
};