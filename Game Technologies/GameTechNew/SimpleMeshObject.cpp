#include "SimpleMeshObject.h"

SimpleMeshObject::SimpleMeshObject(const std::string& name) :
	GameObject(name),
	m_DeleteOnCleanup(false),
	m_pMesh(NULL)
{
}

SimpleMeshObject::~SimpleMeshObject()
{
	if (m_DeleteOnCleanup && m_pMesh)
	{
		delete m_pMesh;
		m_pMesh = NULL;
	}
}

void SimpleMeshObject::SetMesh(Mesh* mesh, bool deleteOnCleanup)
{
	m_pMesh = mesh;
	m_DeleteOnCleanup = deleteOnCleanup;
}

void	SimpleMeshObject::OnRenderObject()
{
	m_pMesh->Draw();
}