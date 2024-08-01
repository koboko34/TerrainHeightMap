#version 410 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 uv;

void main()
{
	gl_Position = vec4(aPos, 0.0, 1.0);
	uv = aTexCoords;
}