#version 410 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D grassTexture;

void main()
{
	FragColor = texture(grassTexture, TexCoord);
	//FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}