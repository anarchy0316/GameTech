#include "CuboidCollisionShape.h"
#include "PhysicsObject.h"
#include "GameObject.h"
#include <nclgl/Matrix3.h>
#include <nclgl/OGLRenderer.h>

Hull CuboidCollisionShape::m_CubeHull = Hull();

CuboidCollisionShape::CuboidCollisionShape()
{
	m_CuboidDimensions = Vector3(1.0f, 1.0f, 1.0f);
	m_CuboidHalfDimensions = Vector3(0.5f, 0.5f, 0.5f);

	m_CuboidAxes.resize(3);
	OnLocalTransformUpdated(); //Constructs the collision axes, which are always the same for a cuboid unless it's local transform is rotated

	if (m_CubeHull.GetNumVertices() == 0)
	{
		ConstructCubeHull();
	}
}

CuboidCollisionShape::CuboidCollisionShape(float width, float height, float depth)
{
	m_CuboidDimensions = Vector3(fabs(width), fabs(height), fabs(depth));
	m_CuboidHalfDimensions = m_CuboidDimensions * 0.5f;

	m_CuboidAxes.resize(3);
	OnLocalTransformUpdated(); //Constructs the collision axes, which are always the same for a cuboid unless it's local transform is rotated


	if (m_CubeHull.GetNumVertices() == 0)
	{
		ConstructCubeHull();
	}
}

CuboidCollisionShape::~CuboidCollisionShape()
{

}

Matrix3 CuboidCollisionShape::BuildInverseInertia(float invMass)
{
	Matrix3 inertia;

	float w2 = m_CuboidDimensions.x * m_CuboidDimensions.x;
	float h2 = m_CuboidDimensions.y * m_CuboidDimensions.y;
	float d2 = m_CuboidDimensions.z * m_CuboidDimensions.z;

	inertia._11 = 12.f * invMass * 1.f / (h2 + d2);
	inertia._22 = 12.f * invMass * 1.f / (w2 + d2);
	inertia._33 = 12.f * invMass * 1.f / (w2 + h2);
	
	return inertia;
}

void CuboidCollisionShape::GetLocalCollisionAxes(PhysicsObject* currentObject, PhysicsObject* collidingObject, std::vector<Vector3>* out_axes)
{
	if (out_axes)
	{
		Matrix3 objOrientation = currentObject->GetOrientation().ToMatrix3();
		out_axes->push_back(objOrientation * Vector3(1.0f, 0.0f, 0.0f)); //X - Axis
		out_axes->push_back(objOrientation * Vector3(0.0f, 1.0f, 0.0f)); //Y - Axis
		out_axes->push_back(objOrientation * Vector3(0.0f, 0.0f, 1.0f)); //Z - Axis
	}
}

void CuboidCollisionShape::GetMinMaxVertexOnAxis(PhysicsObject* currentObject, const Vector3& axis, Vector3* out_min, Vector3* out_max)
{
	Matrix4 wsTransform = currentObject->GetWorldSpaceTransform() * m_LocalTransform * Matrix4::Scale(m_CuboidHalfDimensions);

	Matrix3 invNormalMatrix = Matrix3::Transpose(Matrix3(wsTransform));
	Vector3 local_axis = invNormalMatrix * axis;

	int vMin, vMax;
	m_CubeHull.GetMinMaxVerticesInAxis(local_axis, &vMin, &vMax);

	if (out_min) *out_min = wsTransform * m_CubeHull.GetVertex(vMin).pos;
	if (out_max) *out_max = wsTransform * m_CubeHull.GetVertex(vMax).pos;
}

void CuboidCollisionShape::GetIncidentReferencePolygon(PhysicsObject* currentObject, const Vector3& axis, std::list<Vector3>* out_face, Vector3* out_normal, std::vector<Plane>* out_adjacent_planes)
{
	Matrix4 wsTransform = currentObject->GetWorldSpaceTransform() * m_LocalTransform * Matrix4::Scale(m_CuboidHalfDimensions);

	Matrix3 normalMatrix = Matrix3::Transpose(Matrix3::Inverse(Matrix3(wsTransform)));
	Matrix3 invNormalMatrix = Matrix3::Inverse(normalMatrix);
	Vector3 local_axis = invNormalMatrix * axis;

	int minVertex, maxVertex;
	m_CubeHull.GetMinMaxVerticesInAxis(local_axis, &minVertex, &maxVertex);

	const HullVertex& vert = m_CubeHull.GetVertex(maxVertex);

	const HullFace* best_face = 0;
	float best_correlation = -FLT_MAX;
	for (int faceIdx : vert.enclosing_faces)
	{
		const HullFace* face = &m_CubeHull.GetFace(faceIdx);
		float temp_correlation = Vector3::Dot(local_axis, face->normal);
		if (temp_correlation > best_correlation)
		{
			best_correlation = temp_correlation;
			best_face = face;
		}
	}

	if (out_normal)
	{
		*out_normal = normalMatrix * best_face->normal;
		(*out_normal).Normalise();
	}

	if (out_face)
	{
		for (int vertIdx : best_face->vert_ids)
		{
			const HullVertex& vert = m_CubeHull.GetVertex(vertIdx);
			out_face->push_back(wsTransform * vert.pos);
		}
	}

	if (out_adjacent_planes)
	{
		//Add the reference face itself to the list of adjacent planes
		Vector3 wsPointOnPlane = wsTransform * m_CubeHull.GetVertex(m_CubeHull.GetEdge(best_face->edge_ids[0]).vStart).pos;
		Vector3 planeNrml = -(normalMatrix * best_face->normal);
		planeNrml.Normalise();
		float planeDist = -Vector3::Dot(planeNrml, wsPointOnPlane);

		out_adjacent_planes->push_back(Plane(planeNrml, planeDist));
		

		for (int edgeIdx : best_face->edge_ids)
		{
			const HullEdge& edge = m_CubeHull.GetEdge(edgeIdx);

			wsPointOnPlane = wsTransform * m_CubeHull.GetVertex(edge.vStart).pos;

			for (int adjFaceIdx : edge.enclosing_faces)
			{
				if (adjFaceIdx != best_face->idx)
				{
					const HullFace& adjFace = m_CubeHull.GetFace(adjFaceIdx);

					planeNrml = -(normalMatrix * adjFace.normal);
					planeNrml.Normalise();
					planeDist = -Vector3::Dot(planeNrml, wsPointOnPlane);

					out_adjacent_planes->push_back(Plane(planeNrml, planeDist));

					/*bool valid = p.PointInPlane(wsPointOnPlane);

					//Debug Draw Plane
					Vector3 tangent = Vector3::Cross(planeNrml, *out_normal) * 0.5f;
					Vector3 horiz = Vector3::Cross(planeNrml, tangent);

					OGLRenderer::DrawDebugLine(DEBUGDRAW_PERSPECTIVE, wsPointOnPlane - tangent + horiz, wsPointOnPlane - tangent - horiz, Vector3(1.0f, 0.2f, 1.0f), Vector3(1.0f, 0.2f, 1.0f));
					OGLRenderer::DrawDebugLine(DEBUGDRAW_PERSPECTIVE, wsPointOnPlane + tangent + horiz, wsPointOnPlane + tangent - horiz, Vector3(1.0f, 0.2f, 1.0f), Vector3(1.0f, 0.2f, 1.0f));
					OGLRenderer::DrawDebugLine(DEBUGDRAW_PERSPECTIVE, wsPointOnPlane - tangent + horiz, wsPointOnPlane + tangent + horiz, Vector3(1.0f, 0.2f, 1.0f), Vector3(1.0f, 0.2f, 1.0f));
					OGLRenderer::DrawDebugLine(DEBUGDRAW_PERSPECTIVE, wsPointOnPlane - tangent - horiz, wsPointOnPlane + tangent - horiz, Vector3(1.0f, 0.2f, 1.0f), Vector3(1.0f, 0.2f, 1.0f));
					
					OGLRenderer::DrawDebugLine(DEBUGDRAW_PERSPECTIVE, wsPointOnPlane, wsPointOnPlane + planeNrml * 0.2f, Vector3(1.0f, 0.2f, 1.0f), Vector3(1.0f, 0.2f, 1.0f));*/
				}
			}	
		}
	}
}

void CuboidCollisionShape::OnLocalTransformUpdated()
{
	m_CuboidAxes[0] = Vector3(1.0f, 0.0f, 0.0f); //X - Axis
	m_CuboidAxes[1] = Vector3(0.0f, 1.0f, 0.0f); //Y - Axis
	m_CuboidAxes[2] = Vector3(0.0f, 0.0f, 1.0f); //Z - Axis
}


void CuboidCollisionShape::ConstructCubeHull()
{
	//Vertices
	m_CubeHull.AddVertex(Vector3(-1.0f, -1.0f, -1.0f));		// 0
	m_CubeHull.AddVertex(Vector3(-1.0f,  1.0f, -1.0f));		// 1
	m_CubeHull.AddVertex(Vector3( 1.0f,  1.0f, -1.0f));		// 2
	m_CubeHull.AddVertex(Vector3( 1.0f, -1.0f, -1.0f));		// 3

	m_CubeHull.AddVertex(Vector3(-1.0f, -1.0f,  1.0f));		// 4
	m_CubeHull.AddVertex(Vector3(-1.0f,  1.0f,  1.0f));		// 5
	m_CubeHull.AddVertex(Vector3( 1.0f,  1.0f,  1.0f));		// 6
	m_CubeHull.AddVertex(Vector3( 1.0f, -1.0f,  1.0f));		// 7

	//Faces
	m_CubeHull.AddFace(Vector3(0.0f, 0.0f, -1.0f), 4, new int[]{ 0, 1, 2, 3 });
	m_CubeHull.AddFace(Vector3(0.0f, 0.0f,  1.0f), 4, new int[]{ 4, 5, 6, 7 });
	
	m_CubeHull.AddFace(Vector3( 0.0f,  1.0f, 0.0f), 4, new int[]{ 1, 2, 6, 5 });
	m_CubeHull.AddFace(Vector3( 0.0f, -1.0f, 0.0f), 4, new int[]{ 0, 3, 7, 4 });

	m_CubeHull.AddFace(Vector3( 1.0f, 0.0f, 0.0f), 4, new int[]{ 2, 3, 7, 6 });
	m_CubeHull.AddFace(Vector3(-1.0f, 0.0f, 0.0f), 4, new int[]{ 0, 1, 5, 4 });
}