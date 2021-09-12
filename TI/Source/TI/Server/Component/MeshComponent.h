#pragma once

#include <string>

#include <TI/Server/Component/Component.h>
#include <TI/Server/SceneNode.h>

class Model;
class ResourceManager;

class MeshComponent : public Component, public SceneNode
{
public:
	MeshComponent(ResourceManager* modelManager);
	MeshComponent(const MeshComponent& otherMeshComp);

	MeshComponent(MeshComponent&&) = delete;
	void operator=(const MeshComponent& otherMeshComp) = delete;

	void setParentEntity(Entity* const entity) override;

	void loadModel(const std::string& name);
	Model* const getModel() const;

	std::unique_ptr<Component> clone() const override;

private:
	ResourceManager* modelManager;
	Model* model;
};
