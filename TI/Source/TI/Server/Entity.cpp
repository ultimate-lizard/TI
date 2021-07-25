#include "Entity.h"

#include <TI/Server/Component/TransformComponent.h>
#include <TI/Server/Component/CameraComponent.h>
#include <TI/Renderer/Camera.h>

std::unique_ptr<Entity> Entity::clone() const
{
	auto newEntity = std::make_unique<Entity>();
	newEntity->name = name;
	newEntity->id = id;
	
	for (auto& component : components)
	{
		std::unique_ptr<Component> newComp = component->clone();
		newComp->entity = newEntity.get();
		newEntity->components.push_back(std::move(newComp));
	}

	// TODO: This is bad. Maybe remove TransformComponent at all
	if (auto transformComponent = newEntity->findComponent<TransformComponent>())
	{
		if (auto cameraComponent = newEntity->findComponent<CameraComponent>())
		{
			cameraComponent->getCamera()->setParent(transformComponent);
		}
	}

	return std::move(newEntity);
}
