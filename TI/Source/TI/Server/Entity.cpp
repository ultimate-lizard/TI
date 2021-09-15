#include "Entity.h"

#include <TI/Server/Component/CameraComponent.h>
#include <TI/Renderer/Camera.h>

Entity::Entity(const Entity& otherEntity)
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

void Entity::initComponents()
{
	for (std::unique_ptr<Component>& component : components)
	{
		component->init();
	}
}

std::unique_ptr<Entity> Entity::clone() const
{
	return std::make_unique<Entity>(*this);
}
