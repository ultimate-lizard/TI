#version 330 core

in vec2 texturePos;

out vec4 FragColor;

uniform vec4 triangleColor;
uniform sampler2D textureSampler;

void main()
{
	FragColor = mix(texture(textureSampler, texturePos), triangleColor, triangleColor.w);
}
