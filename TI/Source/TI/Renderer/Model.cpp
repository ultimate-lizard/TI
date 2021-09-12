#include "Model.h"

Model::Model() :
	mesh(std::make_unique<Mesh>()),
	material(std::make_unique<Material>())
{
}

void Model::setMesh(std::unique_ptr<Mesh> mesh)
{
	this->mesh = std::move(mesh);
}

void Model::setMaterial(std::unique_ptr<Material> material)
{
	this->material = std::move(material);
}

Mesh* const Model::getMesh()
{
	return mesh.get();
}

Material* const Model::getMaterial()
{
	return material.get();
}
