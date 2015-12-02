#include "Scene.h"
#include <algorithm>

Scene::Scene(Window& window) : OGLRenderer(window)
{	
	m_DebugShader = new Shader(SHADERDIR"debugVertex.glsl", SHADERDIR"debugFragment.glsl");
	if (!m_DebugShader->LinkProgram()){
		return;
	}

	m_DefaultLightShader = new Shader(SHADERDIR"TechVertex.glsl", SHADERDIR"TechLightFragment.glsl");
	if (!m_DefaultLightShader->LinkProgram()){
		return;
	}

	m_DefaultShadowShader = new Shader(SHADERDIR"TechVertex.glsl", SHADERDIR"TechShadowFragment.glsl");
	if (!m_DefaultShadowShader->LinkProgram()){
		return;
	}

	m_ShadowVolumeShader = new Shader(SHADERDIR"TechVertex.glsl", SHADERDIR"PassThroughFragment.glsl", SHADERDIR"ShadowVolumeGeometry.glsl");
	if (!m_ShadowVolumeShader->LinkProgram()){
		return;
	}


	m_Camera = new Camera();
	m_RootGameObject = new GameObject();
	m_RootGameObject->m_Scene = this;

	m_AmbientColour = Vector3(0.2f, 0.2f, 0.2f);
	m_InvLightDirection = Vector3(0.5f, 1.0f, -0.8f);
	m_SpecularIntensity = 128.0f;

	m_InvLightDirection.Normalise();

	m_ScreenDTex = NULL;
	m_ScreenCTex = NULL;
	m_ScreenFBO = NULL;
	BuildScreenFBO();

	glClearColor(0.6f, 0.6f, 0.6f, 1.f);

	init = true;
}

Scene::~Scene()
{
	if (m_DefaultLightShader)
	{
		delete m_DefaultLightShader;
		m_DefaultLightShader = NULL;
	}

	if (m_DefaultShadowShader)
	{
		delete m_DefaultShadowShader;
		m_DefaultShadowShader = NULL;
	}

	if (m_ShadowVolumeShader)
	{
		delete m_ShadowVolumeShader;
		m_ShadowVolumeShader = NULL;
	}

	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = NULL;
	}

	if (m_RootGameObject)
	{
		delete m_RootGameObject;
		m_RootGameObject = NULL;
	}

	glDeleteTextures(1, &m_ScreenDTex);
	glDeleteTextures(1, &m_ScreenCTex);
	glDeleteFramebuffers(1, &m_ScreenFBO);
}


void Scene::AddGameObject(GameObject* game_object)
{
	m_RootGameObject->AddChildObject(game_object);
}

GameObject* Scene::FindGameObject(const std::string& name)
{
	return m_RootGameObject->FindGameObject(name);
}

void Scene::RenderScene()
{

	//Check to see if the window has been resized
	if (m_ScreenTexWidth != width || m_ScreenTexHeight != height)
	{
		BuildScreenFBO();
	}
	
	//Update all Object's World Transform
	UpdateWorldMatrices(m_RootGameObject, Matrix4());

	//Reset all varying data
	textureMatrix.ToIdentity();
	modelMatrix.ToIdentity();
	viewMatrix = m_Camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(0.01f, 1000.0f, (float)width / (float)height, 45.0f);
	m_FrameFrustum.FromMatrix(projMatrix * viewMatrix);
	

	//Setup Default Shader Uniforms
	Vector3 camPos = m_Camera->GetPosition();
	Vector3 lightPos = m_InvLightDirection * -100.0f;
	Vector4 lightPosEyeSpace = viewMatrix * Vector4(lightPos.x, lightPos.y, lightPos.z, 1.0f);

	SetCurrentShader(m_DefaultLightShader);
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "ambientColour"), 1, &m_AmbientColour.x);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "invLightDir"), 1, &m_InvLightDirection.x);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, &camPos.x);
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "specularIntensity"), m_SpecularIntensity);

	SetCurrentShader(m_DefaultShadowShader);
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "ambientColour"), 1, &m_AmbientColour.x);

	SetCurrentShader(m_ShadowVolumeShader);
	UpdateShaderMatrices();
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "invLightDir"), 1, &m_InvLightDirection.x);


	//Setup Render FBO/OpenGL States
	glBindFramebuffer(GL_FRAMEBUFFER, m_ScreenFBO);
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_CLAMP);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glDepthFunc(GL_LEQUAL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Build Render List
	BuildNodeLists(m_RootGameObject);
	SortNodeLists();

	//Render the Scene in the Dark (As if it were a shadow)
	SetCurrentShader(m_DefaultShadowShader);
	DrawNodes();

	//Render the Shadow Volumes to the Stencil Buffer
	SetCurrentShader(m_ShadowVolumeShader);
	glDepthFunc(GL_LESS);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);

	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
	glStencilOpSeparate(GL_BACK, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

	glDisable(GL_CULL_FACE);
	DrawNodes();
	glEnable(GL_CULL_FACE);
	

	//Finally Render the Light Sections of the scene where the shadow volumes overlapped
	glDepthFunc(GL_LEQUAL);
	glStencilFunc(GL_EQUAL, 0, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);

	SetCurrentShader(m_DefaultLightShader);
	DrawNodes();

	//Clear Render List
	ClearNodeLists();


	//Finally draw all debug data to FBO (this fbo has anti-aliasing and the screen buffer does not)
	glDisable(GL_DEPTH_TEST);
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	SetCurrentShader(m_DebugShader);
	modelMatrix.ToIdentity();
	DrawDebugOrtho();
	DrawDebugPerspective();
	drawnDebugOrtho = true;
	drawnDebugPerspective = true;


	//Present our Screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	PresentScreenFBO();

	//Swap Buffers and get ready to repeat the process
	glUseProgram(0);
	SwapBuffers();
}

void Scene::UpdateScene(float msec)
{
	m_Camera->UpdateCamera(msec);
	UpdateNode(msec, m_RootGameObject);
}


void Scene::BuildScreenFBO()
{
	m_ScreenTexWidth = width;
	m_ScreenTexHeight = height;

	//Generate our Scene Depth Texture
	if (!m_ScreenDTex) glGenTextures(1, &m_ScreenDTex);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_ScreenDTex);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 16, GL_DEPTH24_STENCIL8, width, height, GL_FALSE);

	//Generate our Scene Colour Texture
	if (!m_ScreenCTex) glGenTextures(1, &m_ScreenCTex);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_ScreenCTex);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 16, GL_RGBA8, width, height, GL_FALSE);

	//Generate our Framebuffers
	if (!m_ScreenFBO) glGenFramebuffers(1, &m_ScreenFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_ScreenFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, m_ScreenDTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, m_ScreenDTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_ScreenCTex, 0);

	//Validate our framebuffer
	GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE
		|| !m_ScreenDTex 
		|| !m_ScreenCTex)
	{
		//GL_FRAMEBUFFER_INC
		cout << "Error: Unable to create screen framebuffer object.";
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::PresentScreenFBO()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_ScreenFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void Scene::UpdateWorldMatrices(GameObject* cNode, const Matrix4& parentWM)
{
	cNode->m_WorldTransform = parentWM * cNode->Physics()->GetWorldSpaceTransform() * cNode->m_LocalTransform;

	for (auto child : cNode->GetChildren()) {
		UpdateWorldMatrices(child, cNode->m_WorldTransform);
	}
}

void Scene::BuildNodeLists(GameObject* cNode)
{
	Vector3 obj_pos = cNode->Physics()->GetPosition();
	Vector3 direction = obj_pos - m_Camera->GetPosition();

	FrustrumSortingObject fso;
	fso.camera_distance = Vector3::Dot(direction, direction);
	fso.target			= cNode;

	if (m_FrameFrustum.InsideFrustum(obj_pos, cNode->GetBoundingRadius())) {
		if (cNode->GetColour().w < 1.0f) {
			m_TransparentNodeList.push_back(fso);
		}
		else{
			m_NodeList.push_back(fso);
		}
	}

	for (auto child : cNode->GetChildren()) {
		BuildNodeLists(child);
	}
}


void Scene::SortNodeLists()
{
	std::sort(m_TransparentNodeList.begin(), m_TransparentNodeList.end(), FrustrumSortingObject::CompareByCameraDistance);
	std::sort(m_NodeList.begin(), m_NodeList.end(), FrustrumSortingObject::CompareByCameraDistance);
}

void Scene::ClearNodeLists()
{
	m_TransparentNodeList.clear();
	m_NodeList.clear();
}

void Scene::DrawNodes()
{
	for (auto node : m_NodeList) {
		DrawNode(node.target);
	}

	for (auto node : m_TransparentNodeList) {
		DrawNode(node.target);
	}
}

void Scene::DrawNode(GameObject* n)
{
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*)&n->m_WorldTransform);
	glUniform4fv(glGetUniformLocation(currentShader->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());

	n->OnRenderObject();
}


void Scene::UpdateNode(float msec, GameObject* cNode)
{
	cNode->OnUpdateObject(msec);

	for (auto child : cNode->GetChildren()) {
		UpdateNode(msec, child);
	}
}
