
#pragma once
#include <ncltech\GameObject.h>
#include <nclgl\Mesh.h>

class Player : public GameObject
{
public:
	Player(const std::string& name);
	~Player();


protected:
	virtual void	OnUpdateObject(float dt) override;
	virtual void	OnRenderObject() override;

protected:
	Mesh*			m_pMesh;

};