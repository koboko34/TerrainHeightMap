#version 410 core

out vec4 FragColor;

in vec2 TexCoord;
in float noiseValue;
in float yPos;

uniform sampler2D grassTexture;

void main()
{
	vec4 grassColor = mix(texture(grassTexture, TexCoord), vec4(1.0, 1.0, 0.4, 1.0), 1.0 - pow(1.0 - noiseValue, 3.0));
	
	if (grassColor.a < 1.0)
		discard;

	vec4 fakeAO = vec4(0.5, 0.5, 0.5, 1.0) * grassColor;
	float maxHeightAO = 0.4;
	float alpha = yPos / maxHeightAO;
	alpha = 1.0 - pow(1.0 - alpha, 3);
	alpha = min(alpha, 1.0);

	FragColor = mix(fakeAO, grassColor, alpha);
}