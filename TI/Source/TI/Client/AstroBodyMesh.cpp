#include "AstroBodyMesh.h"

#include <TI/Server/AstroBody.h>

AstroBodyMesh::AstroBodyMesh(AstroBody* astroBody, Model* model) :
	astroBody(astroBody),
	model(model)
{
}

Model* AstroBodyMesh::getModel()
{
	return model;
}
