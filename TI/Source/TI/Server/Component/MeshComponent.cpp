#include "MeshComponent.h"

#include <TI/ModelManager.h>
#include <TI/Server/Component/TransformComponent.h>
#include <TI/Server/Entity.h>

MeshComponent::MeshComponent(Entity* entity, ModelManager* modelManager) :
	Component(entity),
	modelManager(modelManager),
	model(nullptr)
{

}

void MeshComponent::loadModel(const std::string& name)
{
	if (modelManager)
	{
		Model* const model = modelManager->findModel(name);
		this->model = model;
	}
}

Model* const MeshComponent::getModel() const
{
	return model;
}
