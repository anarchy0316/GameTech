/******************************************************************************
Class: NCLDebug
Implements:
Author: Pieran Marris <p.marris@newcastle.ac.uk>
Description:
Encapsulates lots of globally accessible methods to output information to the screen.
Should be able to draw thin/thick lines, circles, text, triangles etc from anywhere in your
program.

This is rendered each frame from within the render section of the scene itself.


Note: Both 'point_radius' (DrawPoint) and 'line_width' (DrawThickLine) are in world-space coordinates
 while 'font_size' (All Text Functions) is in terms of screen pixels - the same as microsoft word etc.


		(\_/)
		( '_')
	 /""""""""""""\=========     -----D
	/"""""""""""""""""""""""\
....\_@____@____@____@____@_/

*//////////////////////////////////////////////////////////////////////////////

#pragma once
#include <nclgl\Matrix4.h>
#include <nclgl\Matrix3.h>
#include <nclgl\Vector4.h>
#include <nclgl\Vector3.h>
#include <nclgl\Shader.h>
#include <vector>
#include <mutex>

#define MAX_LOG_SIZE		25
#define LOG_TEXT_SIZE  		12.0f
#define STATUS_TEXT_SIZE	16.0f

enum TextAlignment
{
	TEXTALIGN_LEFT,
	TEXTALIGN_RIGHT,
	TEXTALIGN_CENTRE
};

struct LogEntry
{
	Vector3		colour;
	std::string text;
};

class NCLDebug
{
	friend class Scene;
public:
	static void DrawPoint(const Vector3& pos, float point_radius, const Vector4& colour = Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	static void DrawThickLine(const Vector3& start, const Vector3& end, float line_width, const Vector4& colour = Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	static void DrawHairLine(const Vector3& start, const Vector3& end, const Vector4& colour = Vector4(1.0f, 1.0f, 1.0f, 1.0f));

	static void DrawMatrix(const Matrix4& transform_mtx);
	static void DrawMatrix(const Matrix3& rotation_mtx, const Vector3& position);

	static void DrawTriangle(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector4& colour = Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	static void DrawPolygon(int n_verts, const Vector3* verts, const Vector4& colour = Vector4(1.0f, 1.0f, 1.0f, 1.0f));

	static void DrawTextClipSpace(const Vector4& pos, const float font_size, const string& text, const TextAlignment alignment = TEXTALIGN_LEFT, const Vector4 colour = Vector4(1.0f, 1.0f, 1.0f, 1.0f));

	static void DrawTextA(const Vector3& pos, const float font_size, const string& text, const TextAlignment alignment = TEXTALIGN_LEFT, const Vector4 colour = Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	static void DrawTextF(const Vector3& pos, const float font_size, const TextAlignment alignment, const Vector4 colour, const string text, ...); ///See "printf" for usuage manual


	static void AddStatusEntry(const Vector4& colour, const std::string text, ...); ///See "printf" for usuage manual
	static void Log(const Vector3& colour, const std::string text, ...); ///See "printf" for usuage manual
	
protected:
	//Called by Scene class
	static void ClearDebugLists();
	static void SortDebugLists();
	static void DrawDebugLists();

	static void LoadShaders();
	static void ReleaseShaders();

	static void SetDebugDrawData(const Matrix4& projViewMatrix, const Vector3& camera_pos)
	{ 
		m_ProjView = projViewMatrix;
		m_CameraPosition = camera_pos;
	}

protected:
	static Vector3	m_CameraPosition;
	static Matrix4	m_ProjView;

	static int m_NumStatusEntries;
	static std::vector<LogEntry> m_LogEntries;
	static int m_LogEntriesOffset;

	static std::vector<Vector4> m_Points;
	static std::vector<Vector4> m_Characters;
	static std::vector<Vector4> m_ThickLines;
	static std::vector<Vector4> m_HairLines;
	static std::vector<Vector4> m_Tris;	

	static Shader*	m_pShaderPoints;
	static Shader*	m_pShaderLines;
	static Shader*	m_pShaderHairLines;
	static Shader*	m_pShaderText;

	static GLuint	m_glArray, m_glBuffer;
	static GLuint	m_glFontTex;


	static std::mutex m_DebugMutex;
};