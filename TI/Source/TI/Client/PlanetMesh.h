#pragma once

#include <TI/Server/SceneNode.h>

class Model;

class PlanetMesh : public SceneNode
{
public:
	PlanetMesh(Model* model);

	Model* getModel();

private:
	Model* model;
};
