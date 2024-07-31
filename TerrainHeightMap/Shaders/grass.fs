#version 410 core

out vec4 FragColor;

in vec2 TexCoord;
in float noiseValue;

uniform sampler2D grassTexture;

void main()
{
	FragColor = mix(texture(grassTexture, TexCoord), vec4(1.0, 1.0, 0.4, 1.0), (1.0 - pow(1.0 - noiseValue, 3.0)));
	
	if (FragColor.a < 1.0)
		discard;
}