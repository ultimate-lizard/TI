#include "TransformComponent.h"

TransformComponent::TransformComponent() :
	SceneNode()
{
}

TransformComponent::TransformComponent(const TransformComponent& other) :
	SceneNode(other)
{

}

std::unique_ptr<Component> TransformComponent::clone() const
{
	return std::make_unique<TransformComponent>(*this);
}
