#include "TransformComponent.h"

TransformComponent::TransformComponent(Entity* entity, const glm::vec3& position, const glm::vec3& rotation) :
	Component(entity),
	position(position),
	pitch(rotation.x),
	yaw(rotation.y),
	roll(rotation.z)
{

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
