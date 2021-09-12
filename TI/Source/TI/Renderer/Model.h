#pragma once

#include <iostream>

#include <glm/glm.hpp>

#include <TI/Renderer/Mesh.h>
#include <TI/Renderer/Material.h>



class Model
{
public:
	void setMesh(Mesh mesh);

	void setMaterial(Material&& material);

	Mesh* const getMesh();
	Material* const getMaterial();

private:
	Mesh mesh;
	Material material;
};

