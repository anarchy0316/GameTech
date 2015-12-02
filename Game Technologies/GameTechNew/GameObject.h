
#pragma once
#include <nclgl\Matrix4.h>
#include "PhysicsObject.h"
#include <vector>

class Scene;
class PhysicsEngine;



class GameObject
{
	//These are the only two classes that can manually set the world transform
	friend class Scene;

public:
	GameObject(const std::string& name = "");
	~GameObject();

	PhysicsObject*		Physics() { return &m_PhysicsObject; }



	const std::string&	GetName()			{ return m_Name; }
	std::vector<GameObject*>& GetChildren() { return m_Children; }
	

	GameObject*			FindGameObject(const std::string& name);
	void				AddChildObject(GameObject* child);



	void			SetLocalTransform(const Matrix4& transform)			{ m_LocalTransform = transform; }
	const Matrix4&  GetLocalTransform()									{ return m_LocalTransform; }


	void			SetColour(const Vector4& colour)	{ m_Colour = colour; }
	const Vector4&	GetColour()							{ return m_Colour; }

	void			SetBoundingRadius(float radius)		{ m_BoundingRadius = radius; }
	float			GetBoundingRadius()					{ return m_BoundingRadius; }

protected:
	virtual void OnRenderObject()				{};				//Handles OpenGL calls to Render the object
	virtual void OnUpdateObject(float msec)		{};				//Override to handle things like AI etc on update loop


protected:
	std::string					m_Name;
	Scene*						m_Scene;
	GameObject*					m_Parent;
	std::vector<GameObject*>	m_Children;

	PhysicsObject				m_PhysicsObject;

	Vector4						m_Colour;
	float						m_BoundingRadius;	//For Frustum Culling
	Matrix4						m_LocalTransform;
	Matrix4						m_WorldTransform;
};