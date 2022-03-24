#version 330 core

in vec2 texturePos;

out vec4 FragColor;

uniform vec4 color;
uniform sampler2D textureSampler;

void main()
{
	vec2 pos = texturePos;
	// pos.x = 0.0f;
	// pos.y = 0.0f;

	FragColor = mix(texture(textureSampler, pos), color, color.w);
}
