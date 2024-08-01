#version 410 core

out vec4 FragColor;

in vec2 uv;

uniform sampler2D colorTexture;
uniform sampler2D depthTexture;

uniform vec4 fogColor;
uniform float fogDensity;
uniform float near;
uniform float far;

void main()
{
	vec4 sceneColor = texture(colorTexture, uv);
	float depth = texture(depthTexture, uv).r;

	float linearDepth = near * far / (far - depth * (far - near));
	
	float exponent = linearDepth * fogDensity;

	float fogFactor = 2.0 - exp(pow(2.0, -fogDensity * linearDepth));
	fogFactor = clamp(fogFactor, 0.0, 1.0);

	FragColor = mix(sceneColor, fogColor, fogFactor);
}