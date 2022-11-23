#include "AstroBodyMesh.h"

#include <TI/Server/CelestialBody.h>

CelestialBodyMesh::CelestialBodyMesh(CelestialBody* astroBody, Model* model) :
	astroBody(astroBody),
	model(model)
{
}

Model* CelestialBodyMesh::getModel()
{
	return model;
}
