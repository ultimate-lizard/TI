#pragma once

#include <glm/glm.hpp>

#include <TI/Renderer/Mesh.h>
#include <TI/Renderer/Material.h>

class Model
{
public:
	Model();

	void setMesh(std::unique_ptr<Mesh> mesh);
	void setMaterial(std::unique_ptr<Material> material);

	Mesh* const getMesh();
	Material* const getMaterial();

private:
	std::unique_ptr<Mesh> mesh;
	std::unique_ptr<Material> material;
};
