
#include "GameObject.h"
#include <nclgl/Mesh.h>

class SimpleMeshObject : public GameObject
{
public:
	SimpleMeshObject(const std::string& name);
	~SimpleMeshObject();

	void SetMesh(Mesh* mesh, bool deleteOnCleanup); //Setting deleteOnCleanup to true will delete the mesh object when this game object is itself deleted.

protected:
	void	OnRenderObject() override;				//Handles OpenGL calls to Render the object

protected:
	bool	m_DeleteOnCleanup;
	Mesh*	m_pMesh;
};