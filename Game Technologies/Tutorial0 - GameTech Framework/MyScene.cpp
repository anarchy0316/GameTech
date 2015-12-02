
#include "MyScene.h"
#include "Player.h"

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



	//Create Ground
	SimpleMeshObject* ground = new SimpleMeshObject("Ground");
	ground->SetMesh(CommonMeshes::Cube(), false);
	ground->SetLocalTransform(Matrix4::Translation(Vector3(0.0, 0.0, 0.0f)) * Matrix4::Scale(Vector3(40.0f, 0.5f, 40.f))); //80m width, 1m height, 80m depth
	ground->SetColour(Vector4(0.2f, 1.0f, 0.5f, 1.0f));
	ground->SetBoundingRadius(80.0f * 80.f);

	this->AddGameObject(ground);



	//Create a Sphere
	SimpleMeshObject* sphere = new SimpleMeshObject("Sphere");
	sphere->SetMesh(CommonMeshes::Sphere(), false);
	sphere->SetLocalTransform(Matrix4::Scale(Vector3(0.5f, 0.5f, 0.5f)));//1m radius
	sphere->SetColour(Vector4(1.0f, 0.2f, 0.5f, 1.0f));
	sphere->SetBoundingRadius(1.0f);

	sphere->Physics()->SetPosition(Vector3(-12.5f, 1.0f, 0.f));

	this->AddGameObject(sphere);



	//Create a Cuboid
	SimpleMeshObject* cube = new SimpleMeshObject("Cube");
	cube->SetMesh(CommonMeshes::Cube(), false);
	cube->SetLocalTransform(Matrix4::Scale(Vector3(0.5f, 0.5f, 0.5f)));	//1m x 1m x 1m
	cube->SetColour(Vector4(0.2f, 0.5f, 1.0f, 1.0f));
	cube->SetBoundingRadius(1.5f);

	cube->Physics()->SetPosition(Vector3(12.5f, 1.0f, 0.0f));

	this->AddGameObject(cube);



	//Create a 'Player'
	Player* player = new Player("Player1");
	player->SetColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	player->SetBoundingRadius(2.0f);

	player->Physics()->SetPosition(Vector3(0.0f, 1.0f, 0.0f));

	this->AddGameObject(player);



	return true;
}

void MyScene::UpdateScene(float msec)
{
	Scene::UpdateScene(msec);

	/*
	Game Update Logic
	*/
	
	GameObject* obj = Scene::FindGameObject("Player1");
	if (obj != NULL)
	{
		Vector3 player_pos = obj->Physics()->GetPosition();
		NCLDebug::Log(Vector3(1.0f, 0.0f, 0.0f), "Player Position - %5.2fm, %5.2fm, %5.2fm\n", player_pos.x, player_pos.y, player_pos.z);
	}

}

void MyScene::RenderScene()
{
	Scene::RenderScene();

	/* 
	Special Rendering Cases (Scene will call all render() functions on GameObjects's is holds)
	*/
}

