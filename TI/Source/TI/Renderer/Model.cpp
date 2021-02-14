#include "Model.h"

void Model::setMesh(Mesh&& mesh)
{
	this->mesh = std::move(mesh);
}

void Model::setMaterial(Material&& material)
{
	this->material = std::move(material);
}

Mesh* const Model::getMesh()
{
	return &mesh;
}

Material* const Model::getMaterial()
{
	return &material;
}
