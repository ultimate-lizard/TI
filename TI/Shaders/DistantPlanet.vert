#version 330 core

layout (location = 0) in vec3 inVertexPos;
layout (location = 1) in vec2 inTexturePos;

out vec2 texturePos;
// out float flogz;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	gl_Position = projection * view * model * vec4(inVertexPos, 1.0f);

//	float Fcoef = 2.0 / log2(10000000.0f + 1.0);
//	gl_Position.z = log2(max(1e-6, 1.0 + gl_Position.w)) * Fcoef - 1.0;
//	flogz = 1.0 + gl_Position.w;
	texturePos = inTexturePos;
}