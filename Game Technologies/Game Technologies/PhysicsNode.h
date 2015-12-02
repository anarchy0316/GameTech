/******************************************************************************
Class:PhysicsNode
Implements:
Author:Rich Davison	<richard.davison4@newcastle.ac.uk>, Callum Rhodes <c.g.rhodes@ncl.ac.uk> and YOU!
Description: This class represents the physical properties of your game's
entities - their position, orientation, mass, collision volume, and so on.
Most of the first few tutorials will be based around adding code to this class
in order to add correct physical integration of velocity / acceleration etc to
your game objects. 


In addition to the tutorial code, this class contains a pointer to a SceneNode.
This pointer is to the 'graphical' representation of your game object, just 
like the SceneNode's used in the graphics module. However, instead of 
calculating positions etc as part of the SceneNode, it should instead be up
to your 'physics' representation to determine - so we calculate a transform
matrix for your SceneNode here, and apply it to the pointer. 

Your SceneNode should still have an Update function, though, in order to
update any graphical effects applied to your object - anything that will end
up modifying a uniform in a shader should still be the job of the SceneNode. 

Note that since the SceneNode can still have children, we can represent our
entire CubeRobot with a single PhysicsNode, and a single SceneNode 'root'.

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////


#pragma once

#include "../../nclgl/Quaternion.h"
#include "../../nclgl/Vector3.h"
#include "../../nclgl/Matrix4.h"
#include "../../nclgl/SceneNode.h"

#include "CollisionVolume.h"

#define LINEAR_VELOCITY_DAMP 0.998
#define ANGULAR_VELOCITY_DAMP 0.998
#define LINEAR_VELOCITY_MIN 0.00001

class PhysicsNode	{
public:
	PhysicsNode(void);
	PhysicsNode(Quaternion orientation, Vector3 position);
	~PhysicsNode(void);

	Vector3		GetPosition() const			{ return m_position;}
	Vector3		GetLinearVelocity() const	{ return m_linearVelocity;}
	
	float		GetInverseMass() const		{return m_invMass;}
	float		GetMass() const				{return 1.0f/m_invMass;}

	Matrix4		GetInverseInertia() const	{ return m_invInertia; }

	Quaternion	GetOrientation() const		{ return m_orientation;}
	Vector3		GetAngularVelocity() const	{ return m_angularVelocity;}

	CollisionVolume* GetCollisionVolume() const { return vol; }

	Matrix4		BuildTransform();

	virtual void		Update(float msec);

	void	SetPosition(Vector3 vec) { m_position = vec; }
	void	SetLinearVelocity(Vector3 vec)		{ m_linearVelocity = vec;}
	
	void	SetMass(float mass) {m_invMass = 1.0f/mass;}
	void	SetInverseMass(float invMass) {m_invMass = invMass;}

	void	SetInverseInertia(Matrix4 invInertia) {m_invInertia = invInertia;}
	
	void	SetOrientation(Quaternion q) { m_orientation = q; }
	void	SetAngularVelocity(Vector3 vec) { m_angularVelocity = vec; }

	void	SetTarget(SceneNode *s) { target = s;}
	void	SetUseGravity(bool value) { useGravity = value; }
	void	SetCollisionVolume(CollisionVolume* vol) { this->vol = vol; }


	Vector3	GetForce()	{ return m_force;}
	Vector3	GetTorque() { return m_torque;}

	void AddForce(Vector3 point, Vector3 force);

	static const Vector3 gravity;

protected:
	bool useGravity;

	//<---------LINEAR-------------->
	Vector3		m_position;
	Vector3		m_linearVelocity;
	Vector3		m_force;
	float		m_invMass;

	//<----------ANGULAR--------------->
	Quaternion  m_orientation;
	Vector3		m_angularVelocity;
	Vector3		m_torque;
	Matrix4     m_invInertia;

	SceneNode*	target;
	CollisionVolume* vol;
};

