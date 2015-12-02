#include "PhysicsObject.h"
#include "PhysicsEngine.h"

PhysicsObject::PhysicsObject()
{
	//Initialise Defaults
	m_wsTransformInvalidated = true;
	m_Enabled	= false;
	m_Type		= PhysicsObjectType::STATIC;

	m_Position = Vector3(0.0f, 0.0f, 0.0f);
	m_LinearVelocity = Vector3(0.0f, 0.0f, 0.0f);
	m_Force = Vector3(0.0f, 0.0f, 0.0f);
	m_InvMass = 0.0f;

	m_Orientation = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
	m_AngularVelocity = Vector3(0.0f, 0.0f, 0.0f);
	m_Torque = Vector3(0.0f, 0.0f, 0.0f);
	m_InvInertia.ToZero();

	m_colShape = NULL;

	Enable();
}

PhysicsObject::~PhysicsObject()
{
	//Delete ColShape
	if (m_colShape != NULL)
	{
		delete m_colShape;
		m_colShape = NULL;
	}

	//Remove From PhysicsEngine
	Disable();
}

void PhysicsObject::Enable()
{
	if (!m_Enabled)
	{
		PhysicsEngine::Instance()->AddPhysicsObject(this);
		m_Enabled = true;
	}
}

void PhysicsObject::Disable()
{
	if (m_Enabled)
	{
		PhysicsEngine::Instance()->RemovePhysicsObject(this);
		m_Enabled = false;
	}
}

const Matrix4& PhysicsObject::GetWorldSpaceTransform()
{
	if (m_wsTransformInvalidated)
	{
		m_wsTransform = m_Orientation.ToMatrix4();
		m_wsTransform.SetPositionVector(m_Position);

		m_wsTransformInvalidated = false;
	}

	return m_wsTransform;
}