#pragma once

#include <nclgl\Vector3.h>
#include <nclgl\Matrix4.h>
#include <vector>

struct HullEdge;
struct HullFace;

struct HullVertex
{
	int idx;
	Vector3 pos;
	std::vector<int> enclosing_edges;
	std::vector<int> enclosing_faces;
};

struct HullEdge
{
	int idx;
	int vStart, vEnd;
	std::vector<int> adjoining_edge_ids;
	std::vector<int> enclosing_faces;
};

struct HullFace
{
	int idx;
	Vector3 normal;
	std::vector<int> vert_ids;
	std::vector<int> edge_ids;
	std::vector<int> adjoining_face_ids;
};

class Hull
{
public:
	Hull();	//Polygon Order means the number of points to form a face, so 3 for a triangle, 4 for a quad and 5 for a pentagon etc
	~Hull();

	void AddVertex(const Vector3& v);
	

	void AddFace(const Vector3& normal, int nVerts, const int* verts);
	void AddFace(const Vector3& normal, const std::vector<int>& vert_ids)		{ AddFace(normal, vert_ids.size(), &vert_ids[0]); }


	int FindEdge(int v0_idx, int v1_idx);
	

	const HullVertex& GetVertex(int idx)		{ return m_Vertices[idx]; }
	const HullEdge& GetEdge(int idx)			{ return m_Edges[idx]; }
	const HullFace& GetFace(int idx)			{ return m_Faces[idx]; }

	size_t GetNumVertices()					{ return m_Vertices.size(); }
	size_t GetNumEdges()					{ return m_Edges.size(); }
	size_t GetNumFaces()					{ return m_Faces.size(); }


	void GetMinMaxVerticesInAxis(const Vector3& local_axis, int* out_min_vert, int* out_max_vert);


protected:
	int ConstructNewEdge(int parent_face_idx, int vert_start, int vert_end); //Called by AddFace
	
protected:
	std::vector<HullVertex>		m_Vertices;
	std::vector<HullEdge>		m_Edges;
	std::vector<HullFace>		m_Faces;

};