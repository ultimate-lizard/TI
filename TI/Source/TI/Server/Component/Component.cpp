#include "Component.h"

Component::Component() :
	entity(nullptr)
{
}

Component::Component(const Component& otherComponent) :
	entity(otherComponent.entity)
{
}

Entity* Component::getParentEntity()
{
	return entity;
}
