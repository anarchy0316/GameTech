#include "Spring.h"
#include "Renderer.h"

void Spring::Update(float msec) {
	Vector3 pos0 = p0->BuildTransform()*localPos0;
	Vector3 pos1 = p1->BuildTransform()*localPos1;

	Vector3 dir = pos1 - pos0;
	float err = dir.Length() - length;
	dir.Normalise();
		
	Vector3 linVel0 = p0->GetLinearVelocity();
	Vector3 linVel1 = p1->GetLinearVelocity();

	Vector3 force = dir*(err*ks - Vector3::Dot(dir, (linVel0 - linVel1)*kd));
		
	p0->AddForce(pos0, force*0.5f);
	p1->AddForce(pos1, -force*0.5f);
}

void Spring::DebugDraw() {
	Vector3 pos0 = p0->BuildTransform()*localPos0;
	Vector3 pos1 = p1->BuildTransform()*localPos1;
	
	Renderer::GetRenderer().DrawDebugCircle(DEBUGDRAW_PERSPECTIVE, pos0, 10, Vector3(1,0,0));
	Renderer::GetRenderer().DrawDebugCircle(DEBUGDRAW_PERSPECTIVE, pos1, 10, Vector3(1,0,0));
	Renderer::GetRenderer().DrawDebugLine(DEBUGDRAW_PERSPECTIVE, pos0, pos1, Vector3(0,1,0), Vector3(0,1,0));
}