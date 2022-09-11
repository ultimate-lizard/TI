#include "MeshComponent.h"

#include <TI/ResourceManager.h>
#include <TI/Server/Entity.h>
#include <TI/Server/Component/TransformComponent.h>

MeshComponent::MeshComponent(ResourceManager* modelManager) :
	Component(),
	SceneMultiNode(),
	modelManager(modelManager),
	model(nullptr)
{

}

MeshComponent::MeshComponent(const MeshComponent& other) :
	Component(other),
	SceneMultiNode(other),
	modelManager(other.modelManager),
	model(other.model)
{
}

std::unique_ptr<Component> MeshComponent::clone() const
{
	return std::make_unique<MeshComponent>(*this);
}

void MeshComponent::loadModel(const std::string& name)
{
	if (modelManager)
	{
		Model* const model = modelManager->getModel(name);
		this->model = model;
	}
}

Model* const MeshComponent::getModel() const
{
	return model;
}
