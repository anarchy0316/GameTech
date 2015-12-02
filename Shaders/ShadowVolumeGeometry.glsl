#version 150 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 11) out;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform vec3  invLightDir;			//Directional Light
uniform float volumeOffset	= 0.1f;	//Amount to move shadow volume along the light direction to prevent depth contension

in Vertex	{
	vec3 worldPos;
	vec2 texCoord;
	vec4 colour;
	vec3 normal;
} IN[];

void main()  
{  
	mat4 projView = projMatrix * viewMatrix;	
	vec3 volOffset = invLightDir * -volumeOffset;
	
	vec4 f_pnt = projView * vec4(-invLightDir, 0.0f);	
	vec4 n_tri[3] = vec4[](
		projView * vec4(IN[0].worldPos + volOffset, 1.0f),
		projView * vec4(IN[1].worldPos + volOffset, 1.0f),
		projView * vec4(IN[2].worldPos + volOffset, 1.0f));

		
	if (dot(IN[0].normal, invLightDir) > -0.0f)
	{	
		//Output Top Cap
		{
			gl_Position = n_tri[0];  
			EmitVertex();
			gl_Position = n_tri[1];  
			EmitVertex();
			gl_Position = n_tri[2];  
			EmitVertex();
			EndPrimitive();			
		}
	
		//Output Extruded Triangles
		{
			gl_Position = n_tri[0];  
			EmitVertex();
			gl_Position = f_pnt;  
			EmitVertex();
			gl_Position = n_tri[1];  
			EmitVertex();
			gl_Position = f_pnt;  
			EmitVertex();
			
			gl_Position = n_tri[2];  
			EmitVertex();
			gl_Position = f_pnt;  
			EmitVertex();
			gl_Position = n_tri[0];  
			EmitVertex();
			gl_Position = f_pnt;  
			EmitVertex();
			EndPrimitive();			
		}
	}
	
}  