#pragma once

#include <memory>

#include <glm/glm.hpp>

#include <TI/Renderer/Mesh.h>

class Material;

class Model
{
public:
	Model(std::unique_ptr<Mesh> mesh, Material* material);
	Model(const Model&) = delete;
	Model(Model&&) = delete;

	void setMaterial(Material* material);

	Mesh* getMesh();
	Material* getMaterial();

private:
	std::unique_ptr<Mesh> mesh;
	Material* material;
};
