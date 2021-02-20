#pragma once

#include <string>

#include <TI/Server/Component/Component.h>

class Model;
class ModelManager;

class MeshComponent : public Component
{
public:
	MeshComponent(ModelManager* modelManager);
	MeshComponent(const MeshComponent& otherMeshComp);

	void loadModel(const std::string& name);

	Model* const getModel() const;

	void operator=(const MeshComponent& otherMeshComp);

	std::unique_ptr<Component> clone() const override;

private:
	ModelManager* modelManager;
	Model* model;
};
