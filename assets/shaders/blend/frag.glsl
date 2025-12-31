#version 410 core

out vec4 FragColor;
in vec2 texCoord;

uniform sampler2D diffuse0;

void main()
{
	if (texture(diffuse0, texCoord).a < 0.1) discard; // discards all fragments with alpha less than 0.1
	FragColor = texture(diffuse0, texCoord); // outputs final color
}