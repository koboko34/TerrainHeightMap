#version 410 core

in float Height;

out vec4 FragColor;

void main()
{
	//float h = (Height + 0.0)/128.0f;
	//FragColor = vec4(h, h, h, 1.0);
	FragColor = vec4(0.8, 0.8, 0.8, 1.0);
}