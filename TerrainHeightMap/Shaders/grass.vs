#version 410 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
//layout (location = 2) in vec3 offsets;

out vec2 TexCoord;
out float noiseValue;
out float yPos;

uniform mat4 models[3];
uniform mat4 view;
uniform mat4 projection;

uniform float FIELD_DISTANCE;
uniform int NUM_GRASS_OBJECTS;
uniform int BILLBOARDS_PER_GRASS;

uniform sampler2D noiseTexture;

float hash(float n)
{
	return fract(sin(n + 137.62591) * 43758.5453123);													// random magic number added to offset when seed == 0
}

float randomAngle(float seed)
{
	return (hash(seed) * 360.0) * (3.14159265 / 180.0);					
}

void main()
{
	float angle = randomAngle(gl_InstanceID / BILLBOARDS_PER_GRASS);
	float sinAngle = sin(angle);
	float cosAngle = cos(angle);

	mat4 rotationMatrix = mat4(
		 cosAngle,	0.0, sinAngle,	0.0,
		 0.0,		1.0, 0.0,		0.0,
		-sinAngle,	0.0, cosAngle,	0.0,
		 0.0,		0.0, 0.0,		1.0
	);

	int objectID = gl_InstanceID / BILLBOARDS_PER_GRASS;												// ID for each grass object

	int root = int(sqrt(NUM_GRASS_OBJECTS));															// dimension of the array, eg. if 400 grass objects -> 20 * 20 grid
	int x = objectID / root;
	int z = objectID % root;

	float xOffset = (x * FIELD_DISTANCE / root) - (FIELD_DISTANCE / 2.0);
	float zOffset = (z * FIELD_DISTANCE / root) - (FIELD_DISTANCE / 2.0);
	vec4 arrayPosOffset = vec4(xOffset, 0.0, zOffset, 0.0);												// position of each grass object on the grid
	
	// random offset to X and Z to break up the uniform positioning of the grass objects
	float diameter = FIELD_DISTANCE / root;
	float rand = hash(objectID);
	float rand2 = hash(rand);
	float xOff = sin(rand) * diameter;
	float zOff = sin(rand2) * diameter;
	vec4 randomOffset = vec4(xOff, 0.0, zOff, 0.0);

	// height offset for each grass object, applied to the top two vertices only (vertex 2 and 3) from a noise texture
	vec4 heightOffset = vec4(0.0);
	noiseValue = 0.0;
	int localVertexID = gl_VertexID % 4;
	if (localVertexID > 1)
	{
		vec2 uv = vec2(float(x) / float(root), float(z) / float(root));
		noiseValue = texture(noiseTexture, uv).x;
		heightOffset = vec4(0.0, noiseValue * 2.0, 0.0, 0.0);
	}

	vec4 worldPos = models[gl_InstanceID % BILLBOARDS_PER_GRASS] * rotationMatrix * vec4(aPos, 0.0, 1.0) + arrayPosOffset + randomOffset + heightOffset;
	
	gl_Position = projection * view * worldPos;
	TexCoord = aTexCoord;
	yPos = aPos.y;
}