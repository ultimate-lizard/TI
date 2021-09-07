#include "TransformComponent.h"

#include <TI/Server/Plane.h>

TransformComponent::TransformComponent() :
	SceneNode(),
	plane(nullptr)
{
}

TransformComponent::TransformComponent(Plane* plane) :
	SceneNode(),
	plane(plane)
{
}

TransformComponent::TransformComponent(const TransformComponent& other) :
	SceneNode(other),
	plane(other.plane)
{
}

std::unique_ptr<Component> TransformComponent::clone() const
{
	return std::make_unique<TransformComponent>(*this);
}

void TransformComponent::setPlane(Plane* plane)
{
	this->plane = plane;
}

Plane* TransformComponent::getPlane() const
{
	return plane;
}
