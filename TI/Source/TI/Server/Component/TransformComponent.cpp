#include "TransformComponent.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

TransformComponent::TransformComponent(const glm::vec3& position, const glm::vec3& rotation) :
	position(position),
	scale({1.0f, 1.0f, 1.0f}),
	pitch(rotation.x),
	yaw(rotation.y),
	roll(rotation.z)
{

}

TransformComponent::TransformComponent(const TransformComponent& otherTransformComp)
{
	position = otherTransformComp.position;
	scale = otherTransformComp.scale;

	pitch = otherTransformComp.pitch;
	yaw = otherTransformComp.yaw;
	roll = otherTransformComp.roll;
}

void TransformComponent::setPosition(const glm::vec3& position)
{
	this->position = position;
}

const glm::vec3& TransformComponent::getPosition()
{
	return position;
}

void TransformComponent::setPitch(float pitch)
{
	this->pitch = pitch;
}

float TransformComponent::getPitch() const
{
	return pitch;
}

void TransformComponent::setYaw(float yaw)
{
	this->yaw = yaw;
}

float TransformComponent::getYaw() const
{
	return yaw;
}

void TransformComponent::setRoll(float roll)
{
	this->roll = roll;
}

float TransformComponent::getRoll() const
{
	return roll;
}

const glm::mat4 TransformComponent::getTransform() const
{
	glm::mat4 transform(1.0f);

	transform = glm::translate(transform, position);
	transform = glm::scale(transform, scale);
	transform = glm::rotate(transform, -glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
	transform = glm::rotate(transform, glm::radians(pitch), glm::vec3(0.0f, 0.0f, 1.0f));
	// TODO: rotate for roll

	return transform;
}

void TransformComponent::setScale(const glm::vec3& scale)
{
	this->scale = scale;
}

const glm::vec3& TransformComponent::getScale() const
{
	return scale;
}

std::unique_ptr<Component> TransformComponent::clone() const
{
	return std::make_unique<TransformComponent>(*this);
}

void TransformComponent::operator=(const TransformComponent& otherTransformComp)
{
	position = otherTransformComp.position;
	scale = otherTransformComp.scale;

	pitch = otherTransformComp.pitch;
	yaw = otherTransformComp.yaw;
	roll = otherTransformComp.roll;
}
