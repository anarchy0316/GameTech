/******************************************************************************
Class: Scene
Implements: OGLRenderer
Author:Pieran Marris <p.marris@newcastle.ac.uk>, Rich Davison <richard.davison4@newcastle.ac.uk> and YOU!
Description: For this module, you are being provided with a basic working
Renderer - to give you more time to work on your physics and AI!

It is basically the Renderer from the Graphics For Games Module as it was
by Tutorial 7 - Scene Management. It will split nodes up into those that are
opaque and transparent, and render accordingly.

The only new bits are the ability to search for Game Object's by their name,
this is not a fast function but does allow some ability to talk between objects in a more
complicated scene.


		(\_/)								-_-_-_-_-_-_-_,------,
		( '_')								_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
	 /""""""""""""\=========     -----D		-_-_-_-_-_-_-~|__( ^ .^) /
	/"""""""""""""""""""""""\				_-_-_-_-_-_-_-""  ""
....\_@____@____@____@____@_/

*//////////////////////////////////////////////////////////////////////////////
#include <nclgl/OGLRenderer.h>
#include <nclgl/Camera.h>
#include <nclgl/Shader.h>
#include <nclgl/Frustum.h>

#include "TSingleton.h"
#include "GameObject.h"

struct FrustrumSortingObject
{
	float		camera_distance;
	GameObject* target;

	static bool CompareByCameraDistance(const FrustrumSortingObject& a, const FrustrumSortingObject& b)  {
		return (a.camera_distance < b.camera_distance) ? true : false;
	}
};

class Scene : public OGLRenderer
{
public:
	Scene(Window& window);
	~Scene();

	void AddGameObject(GameObject* game_object);	
	GameObject* FindGameObject(const std::string& name);

	virtual bool InitialiseGL()				{ return true; };
	virtual void RenderScene();
	virtual void UpdateScene(float dt); //This is msec * 0.001f (e.g time relative to seconds not milliseconds)

protected:
	void	BuildScreenFBO();
	void	PresentScreenFBO();

	void	UpdateWorldMatrices(GameObject* node, const Matrix4& parentWM);

	void	BuildNodeLists(GameObject* node);
	void	SortNodeLists();
	void	ClearNodeLists();
	void	DrawNodes(bool drawTransparentObjects);
	void	DrawNode(GameObject* n);

	void	UpdateNode(float dt, GameObject* n);
protected:
	Camera*				m_Camera;
	Shader*				m_DebugShader;
	Shader			   *m_DefaultLightShader, *m_DefaultShadowShader;
	Shader*				m_ShadowVolumeShader;

	GameObject*			m_RootGameObject;

	Frustum				m_FrameFrustum;
	vector<FrustrumSortingObject> m_TransparentNodeList;
	vector<FrustrumSortingObject> m_NodeList;

	GLuint	m_ScreenTexWidth, m_ScreenTexHeight;
	GLuint  m_ScreenDTex, m_ScreenCTex, m_ScreenFBO;

	Vector3 m_AmbientColour;
	Vector3 m_InvLightDirection;
	float   m_SpecularIntensity;
};