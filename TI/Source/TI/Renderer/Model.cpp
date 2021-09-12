#include "Model.h"

#include <TI/Renderer/Mesh.h>

Model::Model(std::unique_ptr<Mesh> mesh, Material* material) :
	mesh(std::move(mesh)),
	material(material)
{

}

void Model::setMaterial(Material* material)
{
	this->material = material;
}

Mesh* Model::getMesh()
{
	return mesh.get();
}

Material* Model::getMaterial()
{
	return material;
}
