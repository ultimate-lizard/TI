#version 330 core

in vec2 texturePos;

out vec4 FragColor;

uniform vec4 color;
uniform sampler2D textureSampler;

void main()
{
	FragColor = mix(texture(textureSampler, texturePos), color, color.w);
}