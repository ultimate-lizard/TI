#version 330 core

in vec2 texturePos;
in float flogz;

out vec4 FragColor;

uniform vec4 color;
uniform sampler2D textureSampler;

void main()
{
	float Fcoef = 2.0 / log2(10000000.0f + 1.0);
	float Fcoef_half = 0.5 * Fcoef;

    gl_FragDepth = log2(flogz) * Fcoef_half;

	FragColor = mix(texture(textureSampler, texturePos), color, color.w);
}
