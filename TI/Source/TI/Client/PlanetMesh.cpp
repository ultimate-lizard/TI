#include "PlanetMesh.h"

PlanetMesh::PlanetMesh(Model* model) :
	model(model)
{
}

Model* PlanetMesh::getModel()
{
	return model;
}
