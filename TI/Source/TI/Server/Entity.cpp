#include "Entity.h"

#include <TI/Server/Component/CameraComponent.h>
#include <TI/Renderer/Camera.h>

Entity::Entity() :
	SceneNode()
{

}

Entity::Entity(const Entity& otherEntity) :
	SceneNode(otherEntity)
{
	for (const std::unique_ptr<Component>& component : otherEntity.components)
	{
		std::unique_ptr<Component> clonedComponent = component->clone();
		clonedComponent->setParentEntity(this);
		components.push_back(std::move(clonedComponent));
	}

	name = otherEntity.name;
	id = otherEntity.id;
}

std::unique_ptr<Entity> Entity::clone() const
{
	auto newEntity = std::make_unique<Entity>(*this);
	/*
	for (auto& component : components)
	{
		std::unique_ptr<Component> newComp = component->clone();
		newComp->entity = newEntity.get();
		newEntity->components.push_back(std::move(newComp));
	}*/

	return std::move(newEntity);
}
