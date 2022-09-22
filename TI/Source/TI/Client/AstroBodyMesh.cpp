#include "AstroBodyMesh.h"

#include <TI/Server/CelestialBody.h>

AstroBodyMesh::AstroBodyMesh(CelestialBody* astroBody, Model* model) :
	astroBody(astroBody),
	model(model)
{
}

Model* AstroBodyMesh::getModel()
{
	return model;
}
