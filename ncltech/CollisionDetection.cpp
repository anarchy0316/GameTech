#include "CollisionDetection.h"
#include "GameObject.h"
#include <nclgl\OGLRenderer.h>
#include <nclgl\Matrix3.h>
#include <nclgl\Plane.h>

#include "SphereCollisionShape.h"

#include "PhysicsEngine.h"
#include "NCLDebug.h"


bool CollisionDetection::CheckSphereSphereCollision(const PhysicsObject* obj1, const PhysicsObject* obj2, const CollisionShape* shape1, const CollisionShape* shape2, CollisionData* out_coldata) const
{
	/**
	TUTORIAL 5 CODE - Simple Collision Detection Checks
	*/
	return false;
}






void AddPossibleCollisionAxis(Vector3& axis, std::vector<Vector3>* possible_collision_axes)
{
	const float epsilon = 0.0001f;

	//is axis 0,0,0??
	if (Vector3::Dot(axis, axis) < epsilon)
		return;

	axis.Normalise();

	for (const Vector3& p_axis : *possible_collision_axes)
	{
		//Is axis very close to the same as a previous axis already in the list of axes??
		if (Vector3::Dot(axis, p_axis) >= 1.0f - epsilon)
			return;
	}

	possible_collision_axes->push_back(axis);
}

bool CollisionDetection::CheckCollision(const PhysicsObject* obj1, const PhysicsObject* obj2, const CollisionShape* shape1, const CollisionShape* shape2, CollisionData* out_coldata) const
{
	/* 
	TUTORIAL 6 CODE - Seperating Axis Theorem 1
	TUTORIAL 7 CODE - Seperating Axis Theorem 2
	*/
	return false;
}

bool CollisionDetection::CheckCollisionAxis(const Vector3& axis, const PhysicsObject* obj1, const PhysicsObject* obj2, const CollisionShape* shape1, const CollisionShape* shape2, CollisionData* out_coldata) const
{
	/*
	TUTORIAL 6 CODE - Seperating Axis Theorem 1
	*/
	return false;
}


bool CollisionDetection::BuildCollisionManifold(const PhysicsObject* obj1, const PhysicsObject* obj2, const CollisionShape* shape1, const CollisionShape* shape2, const CollisionData& coldata, Manifold* manifold) const
{
	/*
	TUTORIAL 8 CODE - Collision Manifolds
	*/
	return false;
}



/*
Additional geometric functions such as clipping point to a line, clipping edges to a series of planes
*/
Vector3 CollisionDetection::GetClosestPointOnEdges(const Vector3& target, const std::vector<CollisionEdge>& edges) const
{
	Vector3 closest_point, temp_closest_point;
	float closest_distsq = FLT_MAX;

	for (const CollisionEdge& edge : edges)
	{
		Vector3 a_t = target - edge.posA;
		Vector3 a_b = edge.posB - edge.posA;

		float magnitudeAB = Vector3::Dot(a_b, a_b);   //Magnitude of AB vector (it's length squared)     
		float ABAPproduct = Vector3::Dot(a_t, a_b);   //The DOT product of a_to_t and a_to_b     
		float distance = ABAPproduct / magnitudeAB; //The normalized "distance" from a to your closest point  

		if (distance < 0.0f)     //Clamp returned point to be on the line, e.g if the closest point is beyond the AB return either A or B as closest points  
		{
			temp_closest_point = edge.posA;

		}
		else if (distance > 1) {
			temp_closest_point = edge.posB;
		}
		else
		{
			temp_closest_point = edge.posA + a_b * distance;
		}

		Vector3 c_t = target - temp_closest_point;
		float temp_distsq = Vector3::Dot(c_t, c_t);

		if (temp_distsq < closest_distsq)
		{
			closest_distsq = temp_distsq;
			closest_point = temp_closest_point;
		}
	}

	return closest_point;
}

Vector3 CollisionDetection::PlaneEdgeIntersection(const Plane& plane, const Vector3& start, const Vector3& end) const
{
	float start_dist = Vector3::Dot(start, plane.GetNormal()) + plane.GetDistance();
	float end_dist = Vector3::Dot(end, plane.GetNormal()) + plane.GetDistance();

	Vector3 ab = end - start;

	float ab_p = Vector3::Dot(plane.GetNormal(), ab);

	if (fabs(ab_p) > 0.0001f)
	{
		Vector3 p_co = plane.GetNormal() * (-plane.GetDistance());

		Vector3 w = start - p_co;
		float fac = -Vector3::Dot(plane.GetNormal(), w) / ab_p;
		ab = ab * fac;

		return start + ab;
	}

	return start;
}

void CollisionDetection::SutherlandHodgesonClipping(const std::list<Vector3>& input_polygon, int num_clip_planes, const Plane* clip_planes, std::list<Vector3>* out_polygon, bool removePoints) const
{
	if (!out_polygon)
		return;

	std::list<Vector3> ppPolygon1, ppPolygon2;
	std::list<Vector3> *input = &ppPolygon1, *output = &ppPolygon2;

	*output = input_polygon;
	for (int iterations = 0; iterations < num_clip_planes; ++iterations)
	{
		if (output->empty())
			break;

		const Plane& plane = clip_planes[iterations];

		std::swap(input, output);
		output->clear();

		Vector3 startPoint = input->back();
		for (const Vector3& endPoint : *input)
		{
			bool startInPlane = plane.PointInPlane(startPoint);
			bool endInPlane = plane.PointInPlane(endPoint);

			if (removePoints)
			{
				if (endInPlane)
					output->push_back(endPoint);
			}
			else
			{
				//if entire edge is within the clipping plane, keep it as it is
				if (startInPlane && endInPlane)
					output->push_back(endPoint);

				//if edge interesects the clipping plane, cut the edge along clip plane
				else if (startInPlane && !endInPlane)
				{
					output->push_back(PlaneEdgeIntersection(plane, startPoint, endPoint));
				}
				else if (!startInPlane && endInPlane)
				{
					output->push_back(PlaneEdgeIntersection(plane, endPoint, startPoint));					
					output->push_back(endPoint);
				}
			}

			//..otherwise the edge is entirely outside the clipping plane and should be removed


			startPoint = endPoint;
		}
	}

	*out_polygon = *output;
}
