#version 150 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;

uniform vec4 nodeColour;

in  vec3 position;
in  vec2 texCoord;
in  vec3 normal;

out Vertex	{
	vec3 worldPos;
	vec2 texCoord;
	vec4 colour;
	vec3 normal;
} OUT;

void main(void)	{
	vec4 worldPos 	= modelMatrix * vec4(position, 1.0);
	gl_Position		= projMatrix * viewMatrix * worldPos;
	
	OUT.worldPos	= worldPos.xyz;
	OUT.texCoord	= (textureMatrix * vec4(texCoord, 0.0, 1.0)).xy;
	OUT.colour		= nodeColour;
	
	//Below is a much quicker way to calculate the rotated normal value, however it only works
	//  when the model matrix has the same scaling on all axis. If this is not the case, use the method below.
	//OUT.normal		= mat3(modelMatrix) * normal;
	
	// Use this if your objects have different scaling values for the x,y,z axis
	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix )));
	OUT.normal		  = normalMatrix * normal;
	
	
}