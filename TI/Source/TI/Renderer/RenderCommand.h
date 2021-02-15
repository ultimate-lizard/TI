#pragma once

#include <glm/glm.hpp>

class Mesh;
class Material;

struct RenderCommand
{
	Mesh* mesh;
	Material* material;
	glm::mat4 transform;
};
