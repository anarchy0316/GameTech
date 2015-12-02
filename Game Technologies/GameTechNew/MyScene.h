#include <nclgl\Mesh.h>
#include <ncltech\Scene.h>


class MyScene : public Scene
{
public:
	MyScene(Window& window);
	~MyScene();

	bool InitialiseGL()	override;
	void UpdateScene(float msec)  override;

private:
	GLuint m_CheckerboardTex;
	Mesh* m_pQuad;
	Mesh* m_pCube;
	Mesh* m_pCone;
	Mesh* m_pSphere;
};