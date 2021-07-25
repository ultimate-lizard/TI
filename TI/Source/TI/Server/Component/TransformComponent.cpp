#include "TransformComponent.h"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <TI/Server/Component/CameraComponent.h>
#include <TI/Server/Entity.h>
#include <TI/Renderer/Camera.h>

TransformComponent::TransformComponent(const glm::vec3& position, const glm::vec3& rotation)
{
	this->position = position;
	this->rotation = rotation;

	updateTransform();
}

TransformComponent::TransformComponent(const TransformComponent& otherTransformComponent)
{
	position = otherTransformComponent.position;
	rotation = otherTransformComponent.rotation;
	scale = otherTransformComponent.scale;

	parent = otherTransformComponent.parent;
	children = otherTransformComponent.children;

	updateTransform();
}

void TransformComponent::tick(float dt)
{
	std::cout << "Entity pitch: " << rotation.x << std::endl;
	std::cout << "Entity yaw: " << rotation.y << std::endl;
	std::cout << "Entity roll: " << rotation.z << std::endl;
}

std::unique_ptr<Component> TransformComponent::clone() const
{
	return std::make_unique<TransformComponent>(*this);
}

void TransformComponent::operator=(const TransformComponent& otherTransformComponent)
{
	position = otherTransformComponent.position;
	rotation = otherTransformComponent.rotation;
	scale = otherTransformComponent.scale;

	parent = otherTransformComponent.parent;
	children = otherTransformComponent.children;

	updateTransform();
}
