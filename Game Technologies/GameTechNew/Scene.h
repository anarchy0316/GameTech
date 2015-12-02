
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
	virtual void UpdateScene(float msec);

protected:
	void	BuildScreenFBO();
	void	PresentScreenFBO();

	void	UpdateWorldMatrices(GameObject* node, const Matrix4& parentWM);

	void	BuildNodeLists(GameObject* node);
	void	SortNodeLists();
	void	ClearNodeLists();
	void	DrawNodes();
	void	DrawNode(GameObject* n);

	void	UpdateNode(float msec, GameObject* n);
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