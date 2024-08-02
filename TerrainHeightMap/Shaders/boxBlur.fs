#version 410 core

out vec4 FragColor;

in vec2 uv;

uniform sampler2D colorTexture;
uniform vec2 texOffset;
uniform int radius;

void main()
{
	if (radius == 0)
	{
		FragColor = vec4(texture(colorTexture, uv).rgb, 1.0);
		return;
	}

	vec3 blurredColor = vec3(0.0);
	int index = 0;

	for (int i = -radius; i <= radius; i++)
	{
		for (int j = -radius; j <= radius; j++)
		{
			vec2 offset = vec2(float(i), float(j)) * texOffset;
			vec2 finalUv = uv + offset;
			
			if (finalUv.x < 0.0 || finalUv.x > 1.0 || finalUv.y < 0.0 || finalUv.y > 1.0)
				continue;
			
			vec3 neighbourColor = texture(colorTexture, finalUv).rgb;

			blurredColor += neighbourColor;
			index++;
		}
	}

	blurredColor /= float(index);
	FragColor = vec4(blurredColor, 1.0);
}