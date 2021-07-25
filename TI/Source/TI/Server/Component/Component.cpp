#include "Component.h"

Component::Component() :
	entity(nullptr)
{
}

Component::Component(const Component& otherComponent) :
	entity(otherComponent.entity)
{

}

void Component::setParentEntity(Entity* const entity)
{
	this->entity = entity;
}
