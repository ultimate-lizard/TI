#version 330 core

layout (location = 0) in vec3 inVertexPos;
layout (location = 1) in vec3 inNormalPos;
layout (location = 2) in vec2 inTexturePos;

out vec2 texturePos;
out vec3 normal;
out vec3 fragPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	gl_Position = projection * view * model * vec4(inVertexPos, 1.0f);
	texturePos = inTexturePos;
	fragPos = vec3(model * vec4(inVertexPos, 1.0f));
	normal = inNormalPos;
}
