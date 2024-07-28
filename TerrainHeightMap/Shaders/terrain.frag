#version 410 core
out vec4 FragColor;

in vec2 heightMapCoord;
in float yVal;

uniform sampler2D heightMapTexture;

void main()
{
	float remappedY = yVal / 32.0 ;
	
	//FragColor = texture(heightMapTexture, heightMapCoord);
	FragColor = vec4(vec3(remappedY), 1.0);
	//FragColor = vec4(heightMapCoord.x, heightMapCoord.y, 0.0, 1.0);
}