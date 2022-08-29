#version 330 core

in vec2 texturePos;
in float flogz;

out vec4 FragColor;

uniform vec4 color;
uniform sampler2D textureSampler;
uniform float playerDistance;

void main()
{
	float Fcoef = 2.0 / log2(10000000.0f + 1.0);
	float Fcoef_half = 0.5 * Fcoef;

    gl_FragDepth = log2(flogz) * Fcoef_half;

	vec4 result = mix(texture(textureSampler, texturePos), color, color.w);

	float fadeEndDistance = 5.0f;
	float fadeStartDistance = 100.0f;

	float currentValue = playerDistance - fadeStartDistance;

	if (currentValue >= 0.0f)
	{
		if (currentValue > fadeEndDistance)
		{
			currentValue = fadeEndDistance;
		}

		result.w = (1 / fadeEndDistance) * currentValue;
	}
	else
	{
		result.w = 0.0f;
	}

	FragColor = result;
}
