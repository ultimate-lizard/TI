#include "Entity.h"

std::unique_ptr<Entity> Entity::clone() const
{
	auto newEntity = std::make_unique<Entity>();
	newEntity->name = name;
	newEntity->id = id;
	
	for (auto& component : components)
	{
		auto newComp = component->clone();
		newComp->entity = newEntity.get();
		newEntity->components.push_back(std::move(newComp));
	}

	return std::move(newEntity);
}
