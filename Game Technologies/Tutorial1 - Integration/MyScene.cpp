
#include "MyScene.h"

#include <nclgl/OBJMesh.h>

#include <ncltech\SimpleMeshObject.h>
#include <ncltech\PhysicsEngine.h>
#include <ncltech\CommonMeshes.h>
#include <ncltech\NCLDebug.h>


MyScene::MyScene(Window& window) : Scene(window)
{
	if (init == true)
		init = InitialiseGL();

	UpdateWorldMatrices(m_RootGameObject, Matrix4());


	//PhysicsEngine::Instance()->SetUpdateTimestep(1.0f / 1.0f);	//1 Update per second

	//PhysicsEngine::Instance()->SetUpdateTimestep(1.0f / 5.0f);	//5 Updates per second

	PhysicsEngine::Instance()->SetUpdateTimestep(1.0f / 60.0f);		//60 Updates per second
}

MyScene::~MyScene()
{
	if (m_TargetTexture)
	{
		glDeleteTextures(1, &m_TargetTexture);
		m_TargetTexture = NULL;
	}
}

bool MyScene::InitialiseGL()
{
	m_Camera->SetPosition(Vector3(-6.25f, 2.0f, 10.0f));

	PhysicsEngine::Instance()->SetGravity(Vector3(0.0f, 0.0f, 0.0f));		//No Gravity
	PhysicsEngine::Instance()->SetDampingFactor(1.0f);						//No Damping
	

	m_TargetTexture = SOIL_load_OGL_texture(TEXTUREDIR"target.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
	glBindTexture(GL_TEXTURE_2D, m_TargetTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST); //No linear interpolation to get crisp checkerboard no matter the scalling
	glBindTexture(GL_TEXTURE_2D, 0);

		
	//Create Ground
	SimpleMeshObject* ground  = new SimpleMeshObject("Ground");
	ground->SetMesh(CommonMeshes::Cube(), false);
	ground->SetLocalTransform(Matrix4::Scale(Vector3(80.0f, 0.1f, 2.f)));
	ground->SetColour(Vector4(0.2f, 1.0f, 0.5f, 1.0f));
	ground->SetBoundingRadius(80.0f * 80.f);

	ground->Physics()->SetPosition(Vector3(-6.25f, -0.2f, 0.0f));

	this->AddGameObject(ground);
	



	//Create Target
	SimpleMeshObject* target = new SimpleMeshObject("Target");
	target->SetMesh(CommonMeshes::Cube(), false);
	target->SetTexture(m_TargetTexture, false);
	target->SetLocalTransform(Matrix4::Scale(Vector3(0.1f, 2.0f, 2.f)));
	target->SetColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	target->SetBoundingRadius(4.0f);

	target->Physics()->SetPosition(Vector3(0.1f, 2.0f, 0.0f));

	this->AddGameObject(target);
	

	//Create a projectile
	m_Sphere = new SimpleMeshObject("Sphere");
	m_Sphere->SetMesh(CommonMeshes::Sphere(), false);
	m_Sphere->SetLocalTransform(Matrix4::Scale(Vector3(0.5f, 0.5f, 0.5f)));
	m_Sphere->SetColour(Vector4(1.0f, 0.2f, 0.5f, 1.0f));
	m_Sphere->SetBoundingRadius(1.0f);

	m_Sphere->Physics()->SetPosition(Vector3(-12.5f, 2.0f, 0.f));

	m_Sphere->Physics()->SetInverseMass(1.f);

	m_Sphere->Physics()->SetLinearVelocity(Vector3(0.f, 2.5f, 0.0f));
	m_Sphere->Physics()->SetForce(Vector3(1.f, -1.f, 0.0f));

	this->AddGameObject(m_Sphere);

	return true;
}

void MyScene::UpdateScene(float msec)
{
	Scene::UpdateScene(msec);

	if (!PhysicsEngine::Instance()->IsPaused())
	{
		m_TrajectoryPoints.push_back(m_Sphere->Physics()->GetPosition());
	}

	if (m_Sphere->Physics()->GetPosition().y < 0.0f)
	{
		PhysicsEngine::Instance()->SetPaused(true);
	}
}

void MyScene::RenderScene()
{
	for (size_t i = 1; i < m_TrajectoryPoints.size(); i++)
	{
		NCLDebug::DrawHairLine(m_TrajectoryPoints[i - 1], m_TrajectoryPoints[i], Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	}

	Scene::RenderScene();
}

