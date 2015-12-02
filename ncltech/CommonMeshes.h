/******************************************************************************
Class: CommonMeshes
Implements:
Author: Pieran Marris <p.marris@newcastle.ac.uk>
Description: A quick and dirty library of common meshes, to save loading the same meshes over and over again.

These are loaded when the scene is first initialised and released when it is deleted, so will 
be availible for the entirity of the program. Saves loading multiple copies of Cube/Sphere etc when starting the program.


		(\_/)
		( '_')
	 /""""""""""""\=========     -----D
	/"""""""""""""""""""""""\
....\_@____@____@____@____@_/

*//////////////////////////////////////////////////////////////////////////////

#pragma once
#include <nclgl\Mesh.h>

class Scene;

class CommonMeshes
{
	friend class Scene; //Initializes/Destroys the given meshes with it's own lifecycle
public:

	static Mesh* Plane()		{ return m_pPlane; }
	static Mesh* Cube()			{ return m_pCube; }
	static Mesh* Sphere()		{ return m_pSphere; }


protected:
	static void InitializeMeshes();
	static void ReleaseMeshes();

protected:
	static Mesh* m_pCube;
	static Mesh* m_pSphere;
	static Mesh* m_pPlane;


	static GLuint m_CheckerboardTex;
};