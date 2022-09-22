#pragma once

#include <string>

#include <TI/Server/Component/Component.h>
#include <TI/Server/SceneNode.h>

class Model;
class ResourceManager;

class MeshComponent : public Component, public SceneMultiNode
{
public:
	MeshComponent(ResourceManager* modelManager);
	MeshComponent(const MeshComponent& otherMeshComp);

	MeshComponent(MeshComponent&&) = delete;
	void operator=(const MeshComponent& otherMeshComp) = delete;

	std::unique_ptr<Component> clone() const override;

	void loadModel(const std::string& name);
	Model* const getModel() const;

	void setVisibleForParentEntity(bool newVisibility);
	bool isVisibleForParentEntity() const;

private:
	ResourceManager* modelManager;
	Model* model;

	bool visibleForParentEntity;
};
