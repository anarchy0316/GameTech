#version 150 core

uniform sampler2D fontTex;

in Vertex	{
	vec4 colour;
	vec2 texCoords;
} IN;

out vec4 gl_FragColor;

void main(void)	{
	gl_FragColor = texture(fontTex, IN.texCoords) * IN.colour;
	
	if (gl_FragColor.w < 0.1f)
	{
		discard;
	}
}