#include "MeshComponent.h"

#include <TI/ResourceManager.h>
#include <TI/Server/Entity.h>
#include <TI/Server/Component/TransformComponent.h>

MeshComponent::MeshComponent(ResourceManager* modelManager) :
	Component(),
	modelManager(modelManager),
	model(nullptr),
	visibleForParentEntity(true),
	visible(true)
{

}

MeshComponent::MeshComponent(const MeshComponent& other) :
	Component(other),
	modelManager(other.modelManager),
	model(other.model),
	visibleForParentEntity(other.visibleForParentEntity),
	visible(other.visible)
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

void MeshComponent::setVisibleForParentEntity(bool newVisibility)
{
	visibleForParentEntity = newVisibility;
}

bool MeshComponent::isVisibleForParentEntity() const
{
	return visibleForParentEntity;
}

void MeshComponent::setVisibility(bool newVisibility)
{
	visible = newVisibility;
}

bool MeshComponent::isVisible() const
{
	return visible;
}
