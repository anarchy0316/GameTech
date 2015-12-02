
#pragma once

#include <nclgl\Mesh.h>
#include <ncltech\Scene.h>
#include <ncltech\SimpleMeshObject.h>

class MyScene : public Scene
{
public:
	MyScene(Window& window);
	~MyScene();

	bool InitialiseGL()	override;
	void RenderScene() override;
	void UpdateScene(float dt)  override;
protected:
	std::vector<Vector3> m_TrajectoryPoints;
	SimpleMeshObject *m_Sphere;
	GLuint m_TargetTexture;

};