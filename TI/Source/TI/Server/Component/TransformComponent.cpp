#include "TransformComponent.h"

#include <TI/Server/Plane.h>
#include <TI/Client/DebugInformation.h>
#include <TI/Server/Entity.h>

TransformComponent::TransformComponent() :
	Component(),
	SceneNode(),
	plane(nullptr)
{
}

TransformComponent::TransformComponent(const TransformComponent& other) :
	Component(other),
	SceneNode(other),
	plane(other.plane)
{
}

void TransformComponent::tick(float dt)
{
	// drawDebugPoint(position, { 0.0f, 1.0f, 0.0f, 1.0f }, 2.0f, false);
}

void TransformComponent::setPlane(Plane* plane)
{
	this->plane = plane;
}

Plane* TransformComponent::getPlane() const
{
	return plane;
}

std::unique_ptr<Component> TransformComponent::clone() const
{
	return std::make_unique<TransformComponent>(*this);
}
