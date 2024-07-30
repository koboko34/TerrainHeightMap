#version 410 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 offsets;

out vec2 TexCoord;

uniform mat4 models[3];
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vec4 worldPos = models[gl_InstanceID % 3] * vec4(aPos, 0.0, 1.0) + vec4(offsets, 1.0);
	
	gl_Position = projection * view * worldPos;
	//gl_Position = projection * view * models[gl_InstanceID % 3] * vec4(aPos, 0.0, 1.0);
	TexCoord = aTexCoord;
}