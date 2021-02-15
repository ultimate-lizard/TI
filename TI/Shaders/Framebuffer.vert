#version 330 core

layout (location = 0) in vec3 inVertexPos;
layout (location = 1) in vec2 inTexturePos;

out vec2 texturePos;

void main()
{
	gl_Position = vec4(inVertexPos.x, inVertexPos.y, 0.0f, 1.0f);
	texturePos = inTexturePos;
}
