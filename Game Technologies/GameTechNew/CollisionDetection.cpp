#include "CollisionDetection.h"
#include "GameObject.h"
#include <nclgl\OGLRenderer.h>
#include <nclgl\Matrix3.h>
#include <nclgl\Plane.h>

bool CheckCollisionAxis(Vector3 axis, PhysicsObject* obj1, PhysicsObject* obj2, CollisionShape* shape1, CollisionShape* shape2, CollisionData* out_coldata)
{
	Vector3 min1, min2, max1, max2;

	//Get the min/max vertices along the axis from shape1 and shape2
	shape1->GetMinMaxVertexOnAxis(obj1, axis, &min1, &max1);
	shape2->GetMinMaxVertexOnAxis(obj2, axis, &min2, &max2);

	float minCorrelation1 = Vector3::Dot(axis, min1);
	float maxCorrelation1 = Vector3::Dot(axis, max1);
	float minCorrelation2 = Vector3::Dot(axis, min2);
	float maxCorrelation2 = Vector3::Dot(axis, max2);

	if (minCorrelation1 <= minCorrelation2
		&& maxCorrelation1 >= minCorrelation2)
	{
		if (out_coldata)
		{
			out_coldata->normal = axis;
			out_coldata->penetration = minCorrelation2 - maxCorrelation1;
			out_coldata->pointOnPlane = max1 + out_coldata->normal * out_coldata->penetration;
		}

		return true;
	}


	if (minCorrelation2 <= minCorrelation1
		&& maxCorrelation2 > minCorrelation1)
	{
		if (out_coldata)
		{
			out_coldata->normal = -axis;
			out_coldata->penetration = minCorrelation1 - maxCorrelation2;
			out_coldata->pointOnPlane = min1 + out_coldata->normal * out_coldata->penetration;
		}

		return true;
	}


	return false;
}

Vector3 PlaneEdgeIntersection(const Plane& plane, const Vector3& start, const Vector3& end)
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

void SutherlandHodgesonClipping(const std::list<Vector3>& input_polygon, const std::vector<Plane>& clip_planes, std::list<Vector3>* out_polygon)
{
	if (!out_polygon)
		return;

	std::list<Vector3> ppPolygon1, ppPolygon2;
	std::list<Vector3> *input = &ppPolygon1, *output = &ppPolygon2;

	*output = input_polygon;
	int iterations = 0;
	for (const Plane& plane : clip_planes)
	{
		if (output->empty())
			break;

		std::swap(input, output);
		output->clear();

		Vector3 startPoint = input->back();
		for (const Vector3& endPoint : *input)
		{
			bool startInPlane = plane.PointInPlane(startPoint);
			bool endInPlane = plane.PointInPlane(endPoint);

			//if entire edge is within the clipping plane, keep it as it is
			if (startInPlane && endInPlane)
				output->push_back( endPoint );

			//if edge interesects the clipping plane, cut the edge along clip plane
			else if (startInPlane && !endInPlane)
				output->push_back(PlaneEdgeIntersection(plane, startPoint, endPoint));

			else if (!startInPlane && endInPlane)
			{
				output->push_back(PlaneEdgeIntersection(plane, endPoint, startPoint));
				output->push_back(endPoint);
			}
		
			//..otherwise the edge is entirely outside the clipping plane and should be removed


			startPoint = endPoint;
		}
		iterations++;
	}

	*out_polygon = *output;
}

bool CollisionDetection::CheckCollision(PhysicsObject* obj1, PhysicsObject* obj2, CollisionShape* shape1, CollisionShape* shape2, CollisionData* out_coldata)
{
	CollisionData cur_colData;
	CollisionData best_colData;
	best_colData.penetration = -FLT_MAX;

	//Allocate common storage
	Vector3 world_axis;

	//First we need the collision axes of both shapes
	std::vector<Vector3> shape1_axes;
	std::vector<Vector3> shape2_axes;

	shape1->GetLocalCollisionAxes(obj1, obj2, &shape1_axes);
	shape2->GetLocalCollisionAxes(obj2, obj1, &shape2_axes);

	//Check all axes of shape1 to see if a dividing plane can be found between the two shapes
	for (const Vector3& world_axis : shape1_axes)
	{
		if (!CheckCollisionAxis(world_axis, obj1, obj2, shape1, shape2, &cur_colData))
			return false;
		

		if (cur_colData.penetration > best_colData.penetration)
		{
			best_colData = cur_colData;
		}
	}

	//Check all axes of shape2 to see if a dividing plane can be found between the two shapes
	for (const Vector3& world_axis : shape2_axes)
	{
		if (!CheckCollisionAxis(world_axis, obj1, obj2, shape1, shape2, &cur_colData))
			return false;
		

		if (cur_colData.penetration > best_colData.penetration)
		{
			best_colData = cur_colData;
		}
	}



	//Check all combinations (edges) between shape1 and shape2
	for (const Vector3& world_axis1 : shape1_axes)
		for (const Vector3& world_axis2 : shape2_axes)
		{
			world_axis = Vector3::Cross(world_axis2, world_axis1);
			if (Vector3::Dot(world_axis, world_axis) > 0.001f)
			{
				world_axis.Normalise();

				if (!CheckCollisionAxis(world_axis, obj1, obj2, shape1, shape2, &cur_colData))
					return false;
				

				if (cur_colData.penetration > best_colData.penetration)
				{
					best_colData = cur_colData;
				}
			}
		}

	if (out_coldata)
		*out_coldata = best_colData;


	//Otherwise there must have been a collision
	return true;
}


void CollisionDetection::BuildCollisionManifold(float dt, PhysicsObject* obj1, PhysicsObject* obj2, CollisionShape* shape1, CollisionShape* shape2, const CollisionData& coldata, Manifold* manifold)
{
	if (!manifold) 
		return;


	//Get the required face information for the two shapes around the collision normal
	std::list<Vector3>	 polygon1, polygon2;
	Vector3				 normal1, normal2;
	std::vector<Plane>	 adjPlanes1, adjPlanes2;

	shape1->GetIncidentReferencePolygon(obj1, coldata.normal, &polygon1, &normal1, &adjPlanes1);
	shape2->GetIncidentReferencePolygon(obj2, -coldata.normal, &polygon2, &normal2, &adjPlanes2);


	//If either shape1 or shape2 returned a single point, then it must be on a curve and thus the only contact is already availble
	if (polygon1.size() <= 1)
	{
		if (polygon1.size() == 1)
		{
			manifold->AddContact(dt, polygon1.front(), polygon1.front() - coldata.normal * coldata.penetration, coldata.normal, coldata.penetration);
		}
	}
	else if (polygon2.size() == 1)
	{
		manifold->AddContact(dt, polygon2.front() + coldata.normal * coldata.penetration, polygon2.front(), coldata.normal, coldata.penetration);
	}
	else
	{
		//Otherwise use clipping to cut down the incident face to fit inside the reference planes using the surrounding face planes

		bool				 flipped;
		std::list<Vector3>	 *incPolygon;
		Vector3				 *incNormal;
		std::vector<Plane>	 *refAdjPlanes;
		Plane				 refPlane;

		//Get the incident and reference polygons
		if (fabs(Vector3::Dot(coldata.normal, normal1)) <= fabs(Vector3::Dot(coldata.normal, normal2)))
		{
			float planeDist = -Vector3::Dot(normal1, polygon1.front());
			refPlane = Plane(normal1, planeDist);
			refAdjPlanes = &adjPlanes1;

			incPolygon = &polygon2;
			incNormal = &normal2;

			flipped = false;
		}
		else
		{
			float planeDist = -Vector3::Dot(normal2, polygon2.front());
			refPlane = Plane(normal2, planeDist);
			refAdjPlanes = &adjPlanes2;

			incPolygon = &polygon1;
			incNormal = &normal1;

			flipped = true;
		}


		//Clip the Polygon
		SutherlandHodgesonClipping(*incPolygon, *refAdjPlanes, incPolygon);


		//Now we are left with a selection of valid contact points to be used for the manifold
		Vector3 startPoint = incPolygon->back();
		for (const Vector3& endPoint : *incPolygon)
		{
			float contact_penetration;
			Vector3 globalOnA, globalOnB;

			//Calculate distance to ref plane/face
			contact_penetration = -fabs(Vector3::Dot(endPoint, refPlane.GetNormal()) + refPlane.GetDistance());
			contact_penetration = min(contact_penetration, coldata.penetration);

			if (flipped)
			{
				globalOnA = endPoint - coldata.normal * contact_penetration;
				globalOnB = endPoint;
			}
			else
			{
				globalOnA = endPoint;
				globalOnB = endPoint + coldata.normal * contact_penetration;
			}

			contact_penetration = -(fabs(contact_penetration) + fabs(coldata.penetration)) * 0.5f;

			manifold->AddContact(dt, globalOnA, globalOnB, coldata.normal, contact_penetration);

			startPoint = endPoint;
		}

	}
}