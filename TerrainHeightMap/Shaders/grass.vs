#version 410 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 offsets;

out vec2 TexCoord;

uniform mat4 models[3];
uniform mat4 view;
uniform mat4 projection;

float hash(float n)
{
	return fract(sin(n) * 43758.5453123);
}

float randomAngle(float seed)
{
	return (hash(seed) * 360.0) * (3.14159265 / 180.0);
}

void main()
{
	float angle = randomAngle(gl_InstanceID / 3);
	float sinAngle = sin(angle);
	float cosAngle = cos(angle);

	mat4 rotationMatrix = mat4(
		 cosAngle,	0.0, sinAngle,	0.0,
		 0.0,		1.0, 0.0,		0.0,
		-sinAngle,	0.0, cosAngle,	0.0,
		 0.0,		0.0, 0.0,		1.0
	);

	vec4 worldPos = models[gl_InstanceID % 3] * rotationMatrix * vec4(aPos, 0.0, 1.0) + vec4(offsets, 1.0);
	
	gl_Position = projection * view * worldPos;
	TexCoord = aTexCoord;
}