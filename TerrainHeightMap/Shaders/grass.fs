#version 410 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D grassTexture;

void main()
{
	FragColor = texture(grassTexture, TexCoord);
	if (FragColor.a == 0.0)
		discard;
}