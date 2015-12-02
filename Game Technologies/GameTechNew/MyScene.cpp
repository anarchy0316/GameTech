
#include <nclgl/OBJMesh.h>

#include <ncltech\SimpleMeshObject.h>
#include <ncltech\CuboidCollisionShape.h>
#include <ncltech\SphereCollisionShape.h>
#include <ncltech\DistanceConstraint.h>
#include <ncltech\PhysicsEngine.h>

#include "MyScene.h"



MyScene::MyScene(Window& window) :
	Scene(window),
	m_pQuad(NULL),
	m_pCube(NULL),
	m_pSphere(NULL),
	m_pCone(NULL)
{
	if (init == true)
		init = InitialiseGL();

	UpdateWorldMatrices(m_RootGameObject, Matrix4());
}

MyScene::~MyScene()
{
	if (m_pQuad)
	{
		delete m_pQuad;
		m_pQuad = NULL;
	}

	if (m_pCube)
	{
		delete m_pCube;
		m_pCube = NULL;
	}

	if (m_pSphere)
	{
		delete m_pSphere;
		m_pSphere = NULL;
	}
	

	if (m_pCone)
	{
		delete m_pCone;
		m_pCone = NULL;
	}
}

bool MyScene::InitialiseGL()
{
	m_Camera->SetPosition(Vector3(0.0f, 2.0f, 6.0f));

	m_CheckerboardTex = SOIL_load_OGL_texture(TEXTUREDIR"checkerboard.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);

	glBindTexture(GL_TEXTURE_2D, m_CheckerboardTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST); //No linear interpolation to get crisp checkerboard no matter the scalling
	glBindTexture(GL_TEXTURE_2D, 0);

	m_pQuad		= Mesh::GenerateQuadTexCoordCol(Vector2(20.f, 20.f), Vector2(0.0f, 20.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	m_pCube		= new OBJMesh(MESHDIR"cube.obj");
	m_pSphere	= new OBJMesh(MESHDIR"sphere.obj");
	m_pCone		= new OBJMesh(MESHDIR"cone.obj");

	m_pQuad->SetTexture(m_CheckerboardTex);
	m_pCube->SetTexture(m_CheckerboardTex);
	m_pSphere->SetTexture(m_CheckerboardTex);
	m_pCone->SetTexture(m_CheckerboardTex);

	Matrix4 halfScale = Matrix4::Scale(Vector3(0.5f, 0.5f, 0.5f));

	SimpleMeshObject* obj;

	{
		obj = new SimpleMeshObject("Cube1");
		obj->SetMesh(m_pSphere, false);
		obj->SetLocalTransform(halfScale);		
		obj->SetColour(Vector4(0.7f, 0.2f, 0.7f, 1.0f));
		obj->SetBoundingRadius(1.25f);

		obj->Physics()->SetCollisionShape(new SphereCollisionShape(0.5f));

		obj->Physics()->SetInverseMass(1.f);
		obj->Physics()->SetInverseInertia(obj->Physics()->GetCollisionShape()->BuildInverseInertia(1.1f));// obj->Physics()->GetInverseMass()));

		obj->Physics()->SetOrientation(Quaternion::AxisAngleToQuaterion(Vector3(1.0f, 0.0f, 1.0f), 30.f));
		obj->Physics()->SetPosition(Vector3(-2.0f, 1.5f, 0.7f));
		//obj->Physics()->SetLinearVelocity(Vector3(0.0f, 0.0f, -1.5f));
		//obj->Physics()->SetAngularVelocity(Vector3(0.8f, 0.0f, 0.8f));

		
		//obj->SetLocalTransform(Matrix4::Translation(obj->GetPhysicsObject().GetPosition()) * Matrix4::Rotation(45.f, Vector3(1.0f, 0.0f, 0.0f)) * obj->GetLocalTransform());

		this->AddGameObject(obj);
	}



		{
			obj = new SimpleMeshObject("Plane");
			obj->SetMesh(m_pCube, false);
			obj->SetLocalTransform(Matrix4::Scale(Vector3(10.f, 1.0f, 10.f)));
			obj->SetColour(Vector4(0.2f, 0.5f, 1.0f, 1.0f));
			obj->SetBoundingRadius(30.0f);

			obj->Physics()->SetPosition(Vector3(0.0f, -1.0f, 0.0f));
			obj->Physics()->SetCollisionShape(new CuboidCollisionShape(10.0f, 1.0f, 10.0f));

			obj->Physics()->SetFriction(0.0f);

			this->AddGameObject(obj);
		}

		{
			obj = new SimpleMeshObject("Cube2");
			obj->SetMesh(m_pCube, false);
			obj->SetLocalTransform(Matrix4::Scale(Vector3(0.5f, 1.0f, 0.5f)));
			obj->SetColour(Vector4(0.7f, 0.7f, 0.2f, 1.0f));
			obj->SetBoundingRadius(3.25f);

			obj->Physics()->SetEnabled(true);

			obj->Physics()->SetCollisionShape(new CuboidCollisionShape(0.5f, 1.0f, 0.5f));

			obj->Physics()->SetInverseMass(1.0f);
			obj->Physics()->SetInverseInertia(obj->Physics()->GetCollisionShape()->BuildInverseInertia(obj->Physics()->GetInverseMass()));

			obj->Physics()->SetPosition(Vector3(-2.0f, 5.5f, -2.5f));

			obj->Physics()->SetOnCollisionCallback(
				[&](PhysicsObject* obj){
					GameObject* obj2 = m_RootGameObject->FindGameObject("Cube1");
					if (obj->GetGameObject() == obj2)
					{
						printf("Player has hit Cube 2!\n\n");
						return false;
					}
					return true;
				});

			this->AddGameObject(obj);
		}

		{
			obj = new SimpleMeshObject("Cube2");
			obj->SetMesh(m_pCube, false);
			obj->SetLocalTransform(Matrix4::Scale(Vector3(0.5f, 0.5f, 1.0f)));
			obj->SetColour(Vector4(0.7f, 0.7f, 0.2f, 1.0f));
			obj->SetBoundingRadius(3.25f);

			obj->Physics()->SetEnabled(true);

			obj->Physics()->SetCollisionShape(new CuboidCollisionShape(0.5f, 0.5f, 1.0f));

			obj->Physics()->SetInverseMass(1.0f);
			obj->Physics()->SetInverseInertia(obj->Physics()->GetCollisionShape()->BuildInverseInertia(obj->Physics()->GetInverseMass()));

			obj->Physics()->SetPosition(Vector3(-2.0f, 2.5f, -2.5f));

			this->AddGameObject(obj);
		}
			{
				obj = new SimpleMeshObject("Cube3");
				obj->SetMesh(m_pCube, false);
				obj->SetLocalTransform(halfScale);
				obj->SetColour(Vector4(0.7f, 0.7f, 0.2f, 1.0f));
				obj->SetBoundingRadius(1.25f);

				obj->Physics()->SetEnabled(true);

				obj->Physics()->SetCollisionShape(new CuboidCollisionShape(0.5f, 0.5f, 0.5f));

				obj->Physics()->SetInverseMass(0.f);
				obj->Physics()->SetInverseInertia(Matrix3::ZeroMatrix);

				obj->Physics()->SetPosition(Vector3(-2.0f, 0.5f, -6.5f));

				this->AddGameObject(obj);
			}

	SimpleMeshObject *objA, *objB;
	{
		obj = new SimpleMeshObject("Sphere");
		obj->SetMesh(m_pSphere, false);
		obj->SetLocalTransform(halfScale);
		obj->SetColour(Vector4(1.0f, 0.2f, 0.5f, 1.0f));
		obj->SetBoundingRadius(1.0f);

		obj->Physics()->SetEnabled(true);
		obj->Physics()->SetPosition(Vector3(0.0f, 0.5f, 0.0f));
		obj->Physics()->SetCollisionShape(new SphereCollisionShape(0.5f));
		obj->Physics()->SetInverseMass(0.f);
		obj->Physics()->SetInverseInertia(Matrix3::ZeroMatrix);
		this->AddGameObject(obj);
		objA = obj;
	}

	{
		obj = new SimpleMeshObject("Sphere");
		obj->SetMesh(m_pSphere, false);
		obj->SetLocalTransform(halfScale);
		obj->SetColour(Vector4(1.0f, 0.2f, 0.5f, 1.0f));
		obj->SetBoundingRadius(1.0f);

		obj->Physics()->SetEnabled(true);
		obj->Physics()->SetPosition(Vector3(0.0f, 0.5f, 2.0f));
		obj->Physics()->SetCollisionShape(new SphereCollisionShape(0.5f));
		obj->Physics()->SetInverseMass(1.f);
		obj->Physics()->SetInverseInertia(obj->Physics()->GetCollisionShape()->BuildInverseInertia(1.0f));
		this->AddGameObject(obj);
		objB = obj;
	}

	{
		DistanceConstraint* constraint = new DistanceConstraint(objA->Physics(), objB->Physics(), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f,0.0f, 2.0f));
		PhysicsEngine::Instance()->AddConstraint(constraint);

	}



	return true;
}

void MyScene::UpdateScene(float msec)
{
	float dt = msec * 0.001f;
	{
		GameObject* obj = m_RootGameObject->FindGameObject("Cube1");
		if (obj != NULL)
		{
			//obj->SetLocalTransform(obj->GetLocalTransform() * Matrix4::Rotation(15.f * msec * 0.001f, Vector3(1.0f, 0.f, 0.0f)));

			if (Window::GetKeyboard()->KeyDown(KEYBOARD_LEFT))
				obj->Physics()->SetLinearVelocity(obj->Physics()->GetLinearVelocity() + Vector3(-0.5f, 0.0f, 0.0f));
	
			if (Window::GetKeyboard()->KeyDown(KEYBOARD_RIGHT))
				obj->Physics()->SetLinearVelocity(obj->Physics()->GetLinearVelocity() + Vector3(0.5f, 0.0f, 0.0f));

			if (Window::GetKeyboard()->KeyDown(KEYBOARD_UP))
				obj->Physics()->SetLinearVelocity(obj->Physics()->GetLinearVelocity() + Vector3(0.0f, 0.0f, -0.5f));

			if (Window::GetKeyboard()->KeyDown(KEYBOARD_DOWN))
				obj->Physics()->SetLinearVelocity(obj->Physics()->GetLinearVelocity() + Vector3(0.0f, 0.0f, 0.5f));


			/*if (Window::GetKeyboard()->KeyDown(KEYBOARD_LEFT))
				obj->Physics()->SetPosition(obj->Physics()->GetPosition() + Vector3(-0.5f * dt, 0.0f, 0.0f));

			if (Window::GetKeyboard()->KeyDown(KEYBOARD_RIGHT))
				obj->Physics()->SetPosition(obj->Physics()->GetPosition() + Vector3(0.5f * dt, 0.0f, 0.0f));

			if (Window::GetKeyboard()->KeyDown(KEYBOARD_UP))
				obj->Physics()->SetPosition(obj->Physics()->GetPosition() + Vector3(0.0f, 0.5f * dt, 0.0f));

			if (Window::GetKeyboard()->KeyDown(KEYBOARD_DOWN))
				obj->Physics()->SetPosition(obj->Physics()->GetPosition() + Vector3(0.0f, -0.5f * dt, 0.0f));*/
		}
	}

	{
		GameObject* obj = m_RootGameObject->FindGameObject("Cube2");
		if (obj != NULL)
		{
		//	obj->SetLocalTransform(obj->GetLocalTransform() * Matrix4::Rotation(15.f * msec * 0.001f, Vector3(1.0f, 0.f, 0.0f)));
		}
	}

	Scene::UpdateScene(msec);
}