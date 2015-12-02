#include "PhysicsSystem.h"
#include "CollisionHelper.h"

PhysicsSystem* PhysicsSystem::instance = 0;

PhysicsSystem::PhysicsSystem(void)	{

}

PhysicsSystem::~PhysicsSystem(void)	{
	for (int i = 0; i < allSprings.size(); i++) {
		delete allSprings[i];
	}
}

void	PhysicsSystem::Update(float msec) {	
	BroadPhaseCollisions();
	NarrowPhaseCollisions();

	for(vector<Constraint*>::iterator i = allSprings.begin(); i != allSprings.end(); ++i) {
		(*i)->Update(msec);
	}

	for(vector<PhysicsNode*>::iterator i = allNodes.begin(); i != allNodes.end(); ++i) {
		(*i)->Update(msec);
	}
}

void	PhysicsSystem::BroadPhaseCollisions() {

}

void	PhysicsSystem::NarrowPhaseCollisions() {
	for (int i = 0; i < allNodes.size(); i++) {
		PhysicsNode& first = *allNodes[i];
		CollisionVolume* fv = first.GetCollisionVolume();
		if (!fv) continue;
		for (int j = i + 1; j < allNodes.size(); j++) {
			PhysicsNode& second = *allNodes[j];
			CollisionVolume* sv = second.GetCollisionVolume();
			if (!sv) continue;

			switch(fv->GetType()) {
			case COLLISION_VOL_SPHERE:
				switch(sv->GetType()) {
				case COLLISION_VOL_SPHERE:
					CollisionData data;
					if (CollisionHelper::SphereSphereCollision(first, second, &data)) {
						CollisionHelper::AddCollisionImpulse(first, second, data);
					}
					continue;
				}
			case COLLISION_VOL_PLANE:
				switch(sv->GetType()) {
				case COLLISION_VOL_SPHERE:
					CollisionData data;
					if (CollisionHelper::PlaneSphereCollision(first, second, &data)) {
						CollisionHelper::AddCollisionImpulse(first, second, data);
					}
					continue;
				}
			}
		
		}
	}
}

void	PhysicsSystem::AddNode(PhysicsNode* n) {
	allNodes.push_back(n);
}

void	PhysicsSystem::RemoveNode(PhysicsNode* n) {
	for(vector<PhysicsNode*>::iterator i = allNodes.begin(); i != allNodes.end(); ++i) {
		if((*i) == n) {
			allNodes.erase(i);
			return;
		}
	}
}

void	PhysicsSystem::AddConstraint(Constraint* s) {
	allSprings.push_back(s);
}

void	PhysicsSystem::RemoveConstraint(Constraint* c) {
	for(vector<Constraint*>::iterator i = allSprings.begin(); i != allSprings.end(); ++i) {
		if((*i) == c) {
			allSprings.erase(i);
			return;
		}
	}
}

void	PhysicsSystem::AddDebugDraw(DebugDrawer* d) {
	allDebug.push_back(d);
}

void	PhysicsSystem::RemoveDebugDraw(DebugDrawer* d) {
	for(vector<DebugDrawer*>::iterator i = allDebug.begin(); i != allDebug.end(); ++i) {
		if((*i) == d) {
			allDebug.erase(i);
			return;
		}
	}
}

void    PhysicsSystem::DrawDebug() {
	for(vector<DebugDrawer*>::iterator i = allDebug.begin(); i != allDebug.end(); ++i) {
		(*i)->DebugDraw();
	}
}