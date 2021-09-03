#version 330 core

in vec2 texturePos;

out vec4 FragColor;

uniform vec4 color;

void main()
{
	FragColor = color;
}
