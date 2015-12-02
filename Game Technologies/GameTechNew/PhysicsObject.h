#pragma once
#include <nclgl\Quaternion.h>
#include <nclgl\Matrix3.h>
#include "CollisionShape.h"


class PhysicsEngine;

enum PhysicsObjectType
{
	STATIC,					// Means it will not move at all throughout the program
	DYNAMIC,				// Means it will act as a normal physics object
	KINEMATIC				// Means it will be moved via an outside source, such as the objects AI-update script
};

class PhysicsObject
{
	friend class PhysicsEngine;
	friend class GameObject; //Just to allow construction 

public:
	//<--------- GETTERS ------------->
	bool				IsEnabled()					{ return m_Enabled; }
	PhysicsObjectType	GetType()					{ return m_Type; }

	const Vector3&		GetPosition()				{ return m_Position; }
	const Vector3&		GetLinearVelocity()			{ return m_LinearVelocity; }
	const Vector3&		GetForce()					{ return m_Force; }
	const float&		GetInverseMass()			{ return m_InvMass; }

	const Quaternion&	GetOrientation()			{ return m_Orientation; }
	const Vector3&		GetAngularVelocity()		{ return m_AngularVelocity; }
	const Vector3&		GetTorque()					{ return m_Torque; }
	const Matrix3&		GetInverseInertia()			{ return m_InvInertia; }

	CollisionShape*		GetCollisionShape()			{ return m_colShape; }


	//<--------- SETTERS ------------->
	void SetType(const PhysicsObjectType& v)		{ m_Type = v; }

	void SetPosition(const Vector3& v)				{ m_Position = v;	m_wsTransformInvalidated = true; }
	void SetLinearVelocity(const Vector3& v)		{ m_LinearVelocity = v; }
	void SetForce(const Vector3& v)					{ m_Force = v; }
	void SetInverseMass(const float& v)				{ m_InvMass = v; }

	void SetOrientation(const Quaternion& v)		{ m_Orientation = v; m_wsTransformInvalidated = true; }
	void SetAngularVelocity(const Vector3& v)		{ m_AngularVelocity = v; }
	void SetTorque(const Vector3& v)				{ m_Torque = v; }
	void SetInverseInertia(const Matrix3& v)		{ m_InvInertia = v; }

	void SetCollisionShape(CollisionShape* colShape)	{ m_colShape = colShape; }


	//<--------- SPECIAL CASE HANDLERS ------------->
	void SetEnabled(bool enable)					{ enable ? Enable() : Disable(); }
	void Enable();
	void Disable();

	const Matrix4&		GetWorldSpaceTransform();

protected:
	PhysicsObject();
	~PhysicsObject();

protected:
	bool				m_wsTransformInvalidated;
	bool				m_Enabled;
	PhysicsObjectType	m_Type;
	Matrix4				m_wsTransform;

	//<---------LINEAR-------------->
	Vector3		m_Position;
	Vector3		m_LinearVelocity;
	Vector3		m_Force;
	float		m_InvMass;

	//<----------ANGULAR-------------->
	Quaternion  m_Orientation;
	Vector3		m_AngularVelocity;
	Vector3		m_Torque;
	Matrix3     m_InvInertia;

	//<----------COLLISION------------>
	CollisionShape* m_colShape;
};