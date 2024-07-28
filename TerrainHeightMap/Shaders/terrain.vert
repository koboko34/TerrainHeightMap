#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aHeightMapCoord;

out vec2 heightMapCoord;
out float yVal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	heightMapCoord = aHeightMapCoord;
	yVal = aPos.y;
}