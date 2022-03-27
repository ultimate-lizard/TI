#version 330 core

in vec2 texturePos;
in vec3 fragPos;
in vec3 normal;

out vec4 FragColor;

uniform vec4 color;
uniform sampler2D textureSampler;
uniform vec3 lightPos;

void main()
{
	const float ambientStrength = 1.0;
	vec3 lightColor = vec3(1.0f, 1.0f, 0.9f);
	vec3 ambient = ambientStrength * lightColor;

	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - fragPos);
	// vec3 lightDir = normalize(vec3(0.5, 1.0, 0.0));

	float distance = length(lightPos - fragPos);

	float diff = max(dot(norm, lightDir), 0.0f);

	vec3 diffuse = (0.75 + diff / 2) * lightColor;
	// diffuse = pow(diffuse, vec3(0.1));

	float constant = 1.0f;
	float linear = 0.09;
	float quadratic = 0.032;

	float attenuation = 1.0f / (constant + linear * distance + quadratic * (distance * distance));

	ambient *= attenuation;
	diffuse *= attenuation;

	vec3 texColor = texture(textureSampler, texturePos).rgb;
	vec3 lighting = diffuse + ambient;

	texColor *= lighting;

	FragColor = vec4(texColor, 1.0);
}
