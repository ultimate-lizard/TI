#include "MeshComponent.h"

#include <TI/ResourceManager.h>
#include <TI/Server/Entity.h>
#include <TI/Server/Component/TransformComponent.h>

MeshComponent::MeshComponent(ResourceManager* modelManager) :
	Component(),
	SceneNode(),
	modelManager(modelManager),
	model(nullptr)
{

}

MeshComponent::MeshComponent(const MeshComponent& otherMeshComp) :
	Component(otherMeshComp),
	SceneNode(otherMeshComp)
{
	modelManager = otherMeshComp.modelManager;
	model = otherMeshComp.model;
}

void MeshComponent::setParentEntity(Entity* const entity)
{
	Component::setParentEntity(entity);

	if (auto transformComponent = entity->findComponent<TransformComponent>())
	{
		setParentNode(transformComponent);
	}
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

std::unique_ptr<Component> MeshComponent::clone() const
{
	return std::make_unique<MeshComponent>(*this);
}

//void MeshComponent::operator=(const MeshComponent& otherMeshComp)
//{
//	modelManager = otherMeshComp.modelManager;
//	model = otherMeshComp.model;
//}
