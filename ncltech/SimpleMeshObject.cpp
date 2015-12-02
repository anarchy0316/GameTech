#include "SimpleMeshObject.h"

SimpleMeshObject::SimpleMeshObject(const std::string& name) :
	GameObject(name),
	m_DeleteMeshOnCleanup(false),
	m_DeleteTexOnCleanup(false),
	m_pMesh(NULL),
	m_Texture(0)
{
}

SimpleMeshObject::~SimpleMeshObject()
{
	if (m_DeleteMeshOnCleanup && m_pMesh)
	{
		delete m_pMesh;
		m_pMesh = NULL;
	}

	if (m_DeleteTexOnCleanup && m_Texture)
	{
		glDeleteTextures(1, &m_Texture);
		m_Texture = 0;
	}
}

void SimpleMeshObject::SetMesh(Mesh* mesh, bool deleteOnCleanup)
{
	m_pMesh = mesh;
	m_DeleteMeshOnCleanup = deleteOnCleanup;

	if (!m_Texture)
	{
		m_Texture = m_pMesh->GetTexture();
	}
}

void SimpleMeshObject::SetTexture(GLuint tex, bool deleteOnCleanup)
{
	m_Texture = tex;
	m_DeleteTexOnCleanup = deleteOnCleanup;
}

void	SimpleMeshObject::OnRenderObject()
{
	if (m_Texture)
	{
		m_pMesh->SetTexture(m_Texture);
	}

	m_pMesh->Draw();
}