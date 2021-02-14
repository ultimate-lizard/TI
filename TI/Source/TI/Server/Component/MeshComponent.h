#pragma once

#include <string>

#include <TI/Server/Component/Component.h>

class Model;
class ModelManager;

class MeshComponent : public Component
{
public:
	MeshComponent(Entity* entity, ModelManager* modelManager);

	void loadModel(const std::string& name);

	Model* const getModel() const;

private:
	ModelManager* modelManager;
	Model* model;
};
