#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>

class Mesh;
class Material;

enum class RenderMode
{
	Points = GL_POINTS,
	Lines = GL_LINES,
	Triangles = GL_TRIANGLES
};

struct RenderCommand
{
	Mesh* mesh = nullptr;
	Material* material = nullptr;
	glm::mat4 transform = glm::mat4(1.0f);
	unsigned int viewportId = 0;
	RenderMode renderMode = RenderMode::Triangles;
	float lineWidth = 1.0f;
	bool wireframe = false;
	bool cullFaces = true;
	// Multi-draw related fields:
	GLsizei* counts = nullptr;
	void** indices = nullptr;
	GLsizei multiDrawCount = 0;
};
