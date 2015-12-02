#include "NCLDebug.h"
#include <algorithm>

Vector3	NCLDebug::m_CameraPosition;
Matrix4	NCLDebug::m_ProjView;

int NCLDebug::m_NumStatusEntries = 0;
std::vector<LogEntry> NCLDebug::m_LogEntries;
int NCLDebug::m_LogEntriesOffset = 0;

std::vector<Vector4> NCLDebug::m_Points;
std::vector<Vector4> NCLDebug::m_ThickLines;
std::vector<Vector4> NCLDebug::m_HairLines;
std::vector<Vector4> NCLDebug::m_Tris;
std::vector<Vector4> NCLDebug::m_Characters;

Shader*	NCLDebug::m_pShaderPoints = NULL;
Shader*	NCLDebug::m_pShaderLines = NULL;
Shader*	NCLDebug::m_pShaderHairLines = NULL;
Shader*	NCLDebug::m_pShaderText = NULL;

GLuint NCLDebug::m_glArray = NULL;
GLuint NCLDebug::m_glBuffer = NULL;
GLuint NCLDebug::m_glFontTex = NULL;


void NCLDebug::DrawPoint(const Vector3& pos, float point_radius, const Vector4& colour)
{
	m_Points.push_back(Vector4(pos.x, pos.y, pos.z, point_radius));
	m_Points.push_back(colour);
}

void NCLDebug::DrawThickLine(const Vector3& start, const Vector3& end, float line_width, const Vector4& colour)
{
	//For Depth Sorting
	Vector3 midPoint = (start + end) * 0.5f;
	float camDist = Vector3::Dot(midPoint - m_CameraPosition, midPoint - m_CameraPosition);

	//Add to Data Structures
	m_ThickLines.push_back(Vector4(start.x, start.y, start.z, line_width));
	m_ThickLines.push_back(colour);

	m_ThickLines.push_back(Vector4(end.x, end.y, end.z, camDist));
	m_ThickLines.push_back(colour);

	DrawPoint(start, line_width * 0.5f, colour);
	DrawPoint(end, line_width * 0.5f, colour);
}

void NCLDebug::DrawHairLine(const Vector3& start, const Vector3& end, const Vector4& colour)
{
	m_HairLines.push_back(Vector4(start.x, start.y, start.z, 1.0f));
	m_HairLines.push_back(colour);

	m_HairLines.push_back(Vector4(end.x, end.y, end.z, 1.0f));
	m_HairLines.push_back(colour);
}


void NCLDebug::DrawMatrix(const Matrix4& mtx)
{
	Vector3 position = mtx.GetPositionVector();
	DrawHairLine(position, position + Vector3(mtx.values[0], mtx.values[1], mtx.values[2]), Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	DrawHairLine(position, position + Vector3(mtx.values[4], mtx.values[5], mtx.values[6]), Vector4(0.0f, 1.0f, 0.0f, 1.0f));
	DrawHairLine(position, position + Vector3(mtx.values[8], mtx.values[9], mtx.values[10]), Vector4(0.0f, 0.0f, 1.0f, 1.0f));
}

void NCLDebug::DrawMatrix(const Matrix3& mtx, const Vector3& position)
{
	DrawHairLine(position, position + mtx.GetCol(0), Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	DrawHairLine(position, position + mtx.GetCol(1), Vector4(0.0f, 1.0f, 0.0f, 1.0f));
	DrawHairLine(position, position + mtx.GetCol(2), Vector4(0.0f, 0.0f, 1.0f, 1.0f));
}

void NCLDebug::DrawTextClipSpace(const Vector4& cs_pos, const float font_size, const string& text, const TextAlignment alignment, const Vector4 colour)
{
	Vector2 screenSize = Window::GetWindow().GetScreenSize();
	Vector3 cs_size = Vector3(font_size / screenSize.x, font_size / screenSize.y, 0.0f);
	cs_size = cs_size * cs_pos.w;

	//Work out the starting position of text based off desired alignment
	float x_offset = 0.0f;
	int text_len = text.length();

	switch (alignment)
	{
	case TEXTALIGN_RIGHT:
		x_offset = -text_len * cs_size.x * 1.2f;
		break;

	case TEXTALIGN_CENTRE:
		x_offset = -text_len * cs_size.x * 0.6f;
		break;
	}



	//Add each characters to the draw list individually
	for (int i = 0; i < text_len; ++i)
	{
		Vector4 char_pos = Vector4(cs_pos.x + x_offset, cs_pos.y, cs_pos.z, cs_pos.w);
		Vector4 char_data = Vector4(cs_size.x, cs_size.y, (float)(text[i]), 0.0f);

		m_Characters.push_back(char_pos);
		m_Characters.push_back(char_data);
		m_Characters.push_back(colour);
		m_Characters.push_back(colour);	//We dont really need this, but we need the padding to match the same vertex data format as all the other debug drawables

		x_offset += cs_size.x * 1.2f;
	}
}

void NCLDebug::DrawTextF(const Vector3& pos, const float font_size, const TextAlignment alignment, const Vector4 colour, const string text, ...)
{
	va_list args;
	va_start(args, text);

	char buf[1024];
	int needed = vsnprintf_s(buf, 1023, _TRUNCATE, text.c_str(), args);
	va_end(args);

	int length = (needed < 0) ? 1024 : needed;

	std::string formatted_text = std::string(buf, (size_t)length);
	DrawTextA(pos, font_size, formatted_text, alignment, colour);
}


void NCLDebug::DrawTextA(const Vector3& pos, const float font_size, const string& text, const TextAlignment alignment, const Vector4 colour)
{
	//Transform pos to clip space
	Vector4 cs_pos = m_ProjView * Vector4(pos.x, pos.y, pos.z, 1.0f);


	DrawTextClipSpace(cs_pos, font_size, text, alignment, colour);
}


void NCLDebug::DrawTriangle(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector4& colour)
{
	//For Depth Sorting
	Vector3 midPoint = (v0 + v1 + v2) * 0.3333333333f;
	float camDist = Vector3::Dot(midPoint - m_CameraPosition, midPoint - m_CameraPosition);

	//Add to data structures
	m_Tris.push_back(Vector4(v0.x, v0.y, v0.z, camDist));
	m_Tris.push_back(colour);

	m_Tris.push_back(Vector4(v1.x, v1.y, v1.z, 1.0f));
	m_Tris.push_back(colour);

	m_Tris.push_back(Vector4(v2.x, v2.y, v2.z, 1.0f));
	m_Tris.push_back(colour);
}

void NCLDebug::DrawPolygon(int n_verts, const Vector3* verts, const Vector4& colour)
{
	for (int i = 2; i < n_verts; ++i)
	{
		DrawTriangle(verts[0], verts[i - 1], verts[i], colour);
	}
}

void NCLDebug::AddStatusEntry(const Vector4& colour, const std::string text, ...)
{
	float cs_size_x = STATUS_TEXT_SIZE / Window::GetWindow().GetScreenSize().x * 2.0f;
	float cs_size_y = STATUS_TEXT_SIZE / Window::GetWindow().GetScreenSize().y * 2.0f;

	va_list args;
	va_start(args, text);

	char buf[1024];
	int needed = vsnprintf_s(buf, 1023, _TRUNCATE, text.c_str(), args);
	va_end(args);

	int length = (needed < 0) ? 1024 : needed;

	std::string formatted_text = std::string(buf, (size_t)length);

	DrawTextClipSpace(Vector4(-1.0f + cs_size_x * 0.5f, 1.0f - (m_NumStatusEntries * cs_size_y) - cs_size_y, 0.0f, 1.0f), STATUS_TEXT_SIZE, formatted_text, TEXTALIGN_LEFT, colour);
	m_NumStatusEntries++;
}

void NCLDebug::Log(const Vector3& colour, const std::string text, ...)
{
	va_list args;
	va_start(args, text);

	char buf[1024];
	int needed = vsnprintf_s(buf, 1023, _TRUNCATE, text.c_str(), args);
	va_end(args);

	int length = (needed < 0) ? 1024 : needed;

	std::string formatted_text = std::string(buf, (size_t)length);
	
	LogEntry le;
	le.text = formatted_text;
	le.colour = colour;

	if (m_LogEntries.size() < MAX_LOG_SIZE)
		m_LogEntries.push_back(le);
	else
	{
		m_LogEntries[m_LogEntriesOffset] = le;
		m_LogEntriesOffset = (m_LogEntriesOffset + 1) % MAX_LOG_SIZE;
	}
}





void NCLDebug::ClearDebugLists()
{
	m_Points.clear();
	m_ThickLines.clear();
	m_HairLines.clear();
	m_Tris.clear();
	m_Characters.clear();
	m_NumStatusEntries = 0;
}

struct PointVertex
{
	Vector4 pos;
	Vector4 col;
};

struct LineVertex
{
	PointVertex p0;
	PointVertex p1;
};

struct TriVertex
{
	PointVertex p0;
	PointVertex p1;
	PointVertex p2;
};

void NCLDebug::SortDebugLists()
{
	//Sort Points
	if (!m_Points.empty())
	{
		PointVertex* points = reinterpret_cast<PointVertex*>(&m_Points[0].x);
		std::sort(points, points + m_Points.size() / 2, [&](const PointVertex& a, const PointVertex& b)
		{
			float a2 = Vector3::Dot(a.pos.ToVector3() - m_CameraPosition, a.pos.ToVector3() - m_CameraPosition);
			float b2 = Vector3::Dot(b.pos.ToVector3() - m_CameraPosition, b.pos.ToVector3() - m_CameraPosition);
			return (a2 > b2);
		});
	}

	//Sort Lines
	if (!m_ThickLines.empty())
	{
		LineVertex* lines = reinterpret_cast<LineVertex*>(&m_ThickLines[0].x);
		std::sort(lines, lines + m_ThickLines.size() / 4, [](const LineVertex& a, const LineVertex& b)
		{
			return (a.p1.pos.w > b.p1.pos.w);
		});
	}

	//Sort Triangles
	if (!m_Tris.empty())
	{
		TriVertex* tris = reinterpret_cast<TriVertex*>(&m_Tris[0].x);
		std::sort(tris, tris + m_Tris.size() / 6, [](const TriVertex& a, const TriVertex& b)
		{
			return (a.p0.pos.w > b.p0.pos.w);
		});
	}
}

void NCLDebug::DrawDebugLists()
{

	//Draw log text
	float cs_size_x = LOG_TEXT_SIZE / Window::GetWindow().GetScreenSize().x * 2.0f;
	float cs_size_y = LOG_TEXT_SIZE / Window::GetWindow().GetScreenSize().y * 2.0f;
	size_t log_len = m_LogEntries.size();
	for (size_t i = 0; i < log_len; ++i)
	{
		size_t idx = (i + m_LogEntriesOffset) % MAX_LOG_SIZE;
		float alpha = ((m_LogEntries.size() - i) / (float(MAX_LOG_SIZE)));
		alpha = 1.0f - (alpha * alpha);
		Vector3& col3 = m_LogEntries[idx].colour;
		DrawTextClipSpace(Vector4(-1.0f + cs_size_x * 0.5f, -1.0f + ((log_len - i - 1) * cs_size_y) + cs_size_y, 0.0f, 1.0f), LOG_TEXT_SIZE, m_LogEntries[idx].text, TEXTALIGN_LEFT, Vector4(col3.x, col3.y, col3.z, alpha));
	}


	if (!m_glArray)
	{
		printf("NCLDebug Error: Was unable to load all shaders properly!!!\n");
		return;
	}

	//Buffer all data into the single buffer object
	size_t max_size = 0;
	max_size += m_Points.size() + m_ThickLines.size() + m_HairLines.size() + m_Tris.size() + m_Characters.size();
	max_size *= sizeof(Vector4);

	size_t offset_thicklines = m_Points.size();
	size_t offset_hairlines = offset_thicklines + m_ThickLines.size();
	size_t offset_tris = offset_hairlines + m_HairLines.size();
	size_t offset_chars = offset_tris + m_Tris.size();

	const size_t stride = 2 * sizeof(Vector4);

	glBindVertexArray(m_glArray);
	glBindBuffer(GL_ARRAY_BUFFER, m_glBuffer);
	glBufferData(GL_ARRAY_BUFFER, max_size, NULL, GL_STATIC_DRAW);

	glVertexAttribPointer(VERTEX_BUFFER, 4, GL_FLOAT, GL_FALSE, stride, (void*)(0));
	glEnableVertexAttribArray(VERTEX_BUFFER);
	glVertexAttribPointer(COLOUR_BUFFER, 4, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(Vector4)));
	glEnableVertexAttribArray(COLOUR_BUFFER);

	if (!m_Points.empty()) glBufferSubData(GL_ARRAY_BUFFER, 0, m_Points.size() * sizeof(Vector4), &m_Points[0].x);
	if (!m_ThickLines.empty()) glBufferSubData(GL_ARRAY_BUFFER, offset_thicklines * sizeof(Vector4), m_ThickLines.size() * sizeof(Vector4), &m_ThickLines[0].x);
	if (!m_HairLines.empty()) glBufferSubData(GL_ARRAY_BUFFER, offset_hairlines * sizeof(Vector4), m_HairLines.size() * sizeof(Vector4), &m_HairLines[0].x);
	if (!m_Tris.empty()) glBufferSubData(GL_ARRAY_BUFFER, offset_tris * sizeof(Vector4), m_Tris.size() * sizeof(Vector4), &m_Tris[0].x);
	if (!m_Characters.empty()) glBufferSubData(GL_ARRAY_BUFFER, offset_chars * sizeof(Vector4), m_Characters.size() * sizeof(Vector4), &m_Characters[0].x);

	Vector2 screen_size = Window::GetWindow().GetScreenSize();
	float aspectRatio = screen_size.y / screen_size.x;


	if (m_pShaderText && m_Characters.size() > 0)
	{
		glUseProgram(m_pShaderText->GetProgram());
		glUniform1i(glGetUniformLocation(m_pShaderText->GetProgram(), "fontTex"), 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_glFontTex);

		glDrawArrays(GL_LINES, offset_chars >> 1, m_Characters.size() >> 1);
	}


	if (m_pShaderPoints && m_Points.size() > 0)
	{
		glUseProgram(m_pShaderPoints->GetProgram());
		glUniformMatrix4fv(glGetUniformLocation(m_pShaderPoints->GetProgram(), "projViewMatrix"), 1, GL_FALSE, &m_ProjView.values[0]);
		glUniform1f(glGetUniformLocation(m_pShaderPoints->GetProgram(), "pix_scalar"), aspectRatio);

		glDrawArrays(GL_POINTS, 0, m_Points.size() >> 1);
	}


	if (m_pShaderLines && m_ThickLines.size() > 0)
	{
		glUseProgram(m_pShaderLines->GetProgram());
		glUniformMatrix4fv(glGetUniformLocation(m_pShaderLines->GetProgram(), "projViewMatrix"), 1, GL_FALSE, &m_ProjView.values[0]);
		glUniform1f(glGetUniformLocation(m_pShaderLines->GetProgram(), "pix_scalar"), aspectRatio);

		glDrawArrays(GL_LINES, offset_thicklines >> 1, m_ThickLines.size() >> 1);
	}


	if (m_pShaderHairLines && (m_HairLines.size() + m_Tris.size()) > 0)
	{
		glUseProgram(m_pShaderHairLines->GetProgram());
		glUniformMatrix4fv(glGetUniformLocation(m_pShaderHairLines->GetProgram(), "projViewMatrix"), 1, GL_FALSE, &m_ProjView.values[0]);

		if (!m_HairLines.empty()) glDrawArrays(GL_LINES, offset_hairlines >> 1, m_HairLines.size() >> 1);
		if (!m_Tris.empty()) glDrawArrays(GL_TRIANGLES, offset_tris >> 1, m_Tris.size() >> 1);
	}
}

void NCLDebug::LoadShaders()
{
	m_pShaderPoints = new Shader(SHADERDIR"DebugShaders/Vertex.glsl", SHADERDIR"DebugShaders/Fragment.glsl", SHADERDIR"DebugShaders/PointGeometry.glsl");
	if (!m_pShaderPoints->LinkProgram())
	{
		return;
	}

	m_pShaderLines = new Shader(SHADERDIR"DebugShaders/Vertex.glsl", SHADERDIR"DebugShaders/Fragment.glsl", SHADERDIR"DebugShaders/LineGeometry.glsl");
	if (!m_pShaderLines->LinkProgram())
	{
		return;
	}

	m_pShaderHairLines = new Shader(SHADERDIR"DebugShaders/PassThroughVertex.glsl", SHADERDIR"DebugShaders/Fragment.glsl");
	if (!m_pShaderHairLines->LinkProgram())
	{
		return;
	}

	m_pShaderText = new Shader(SHADERDIR"DebugShaders/TextVertex.glsl", SHADERDIR"DebugShaders/TextFragment.glsl", SHADERDIR"DebugShaders/TextGeometry.glsl");
	if (!m_pShaderText->LinkProgram())
	{
		return;
	}

	//Create Buffers
	glGenVertexArrays(1, &m_glArray);
	glGenBuffers(1, &m_glBuffer);

	//Load Font Texture
	m_glFontTex = SOIL_load_OGL_texture(TEXTUREDIR"font1.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT);

}

void NCLDebug::ReleaseShaders()
{
	if (m_pShaderPoints)
	{
		delete m_pShaderPoints;
		m_pShaderPoints = NULL;
	}

	if (m_pShaderLines)
	{
		delete m_pShaderLines;
		m_pShaderLines = NULL;
	}

	if (m_pShaderHairLines)
	{
		delete m_pShaderHairLines;
		m_pShaderHairLines = NULL;
	}

	if (m_pShaderText)
	{
		delete m_pShaderText;
		m_pShaderText = NULL;
	}

	if (m_glArray)
	{
		glDeleteVertexArrays(1, &m_glArray);
		glDeleteBuffers(1, &m_glBuffer);
		m_glArray = NULL;
	}

	if (m_glFontTex)
	{
		glDeleteTextures(1, &m_glFontTex);
		m_glFontTex = NULL;
	}
}

