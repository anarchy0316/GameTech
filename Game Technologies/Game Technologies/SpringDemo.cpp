#include "SpringDemo.h"
#include "Renderer.h"
#include "PhysicsNode.h"

float randFloat(int start, int end) {
	float r = rand()*1.0/RAND_MAX;
	return (end - start)*r + start;
}

SpringDemo::SpringDemo() {
	numBalls = 9;
	mass = 1.0f;
	ks = 0.0001f;
	kd = 0.001f;

	for (int i = 0; i < numBalls; i++) {
		positions.push_back(Vector3(0,0,0));
		velocities.push_back(Vector3(0,0,0));
		forces.push_back(Vector3(0,0,0));
	}
	
	positions[0] = Vector3(100, 200, 0);
	positions[numBalls - 1] = Vector3(500, 200, 0);

	for (int i = 1; i < numBalls - 1; i++) {
		positions[i] = Vector3(randFloat(0, 450), 100, randFloat(0, 90));
	}
}

void SpringDemo::Update(float msec) {
	// Update forces on the centre balls
	for (int i = 1; i < numBalls - 1; i++) {
		Vector3 f0 = (positions[i-1] - positions[i])*ks;
		Vector3 f1 = (positions[i+1] - positions[i])*ks;
		Vector3 f = f0 + f1;
		forces[i] = f - velocities[i]*kd + PhysicsNode::gravity*mass;
	}
	// Intergrate velocity and positions
	float invMass = 1.0f / mass;
	for (int i = 1; i < numBalls - 1; i++) {
		Vector3 a = forces[i] * invMass;
		velocities[i] = velocities[i] + a*msec;
		positions[i] = positions[i] + velocities[i]*msec;
	}
}

void SpringDemo::DebugDraw() {
	Renderer::GetRenderer().DrawDebugCross(DEBUGDRAW_PERSPECTIVE, positions[0], Vector3(10,10,10), Vector3(1,0,0));
	Renderer::GetRenderer().DrawDebugCross(DEBUGDRAW_PERSPECTIVE, positions[numBalls-1], Vector3(10,10,10), Vector3(1,0,0));
	for(int i = 1; i < numBalls-1; i++) {
		Renderer::GetRenderer().DrawDebugCircle(DEBUGDRAW_PERSPECTIVE, positions[i], 10.0f, Vector3(0,0,1));
	}
	for(int i = 0; i < numBalls - 1; i++) {
		Renderer::GetRenderer().DrawDebugLine(DEBUGDRAW_PERSPECTIVE, positions[i], positions[i+1], Vector3(0,0,1), Vector3(1,0,1));
	}
}