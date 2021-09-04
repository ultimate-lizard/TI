#include "MovementComponent.h"

#include <iostream>

#include <TI/Server/Entity.h>
#include <TI/Server/Component/CameraComponent.h>
#include <TI/Renderer/Camera.h>
#include <TI/Server/Component/TransformComponent.h>

MovementComponent::MovementComponent(TransformComponent* const transformComponent) :
	transformComponent(transformComponent),
	yawRate(0.0f),
	pitchRate(0.0f),
	speed(70.0f),
	forward(0.0f, 0.0f, -1.0f),
	up({ 0.0f, 1.0f, 0.0f }),
	right({ 1.0f, 0.0f, 0.0f }),
	sensivity(20.0f)
{

}

MovementComponent::MovementComponent(const MovementComponent& otherMovementComp)
{
	transformComponent = otherMovementComp.transformComponent;

	yawRate = otherMovementComp.yawRate;
	pitchRate = otherMovementComp.pitchRate;

	velocity = otherMovementComp.velocity;
	speed = otherMovementComp.speed;

	forward = otherMovementComp.forward;
	up = otherMovementComp.up;
	right = otherMovementComp.right;

	sensivity = otherMovementComp.sensivity;
}

void MovementComponent::tick(float dt)
{
	if (!transformComponent)
	{
		return;
	}

	glm::vec3 rotation = transformComponent->getRotation();

	rotation.x += pitchRate * dt;
	rotation.y += yawRate * dt;

	if (rotation.x > 89.0f)
	{
		rotation.x = 89.0f;
	}
	if (rotation.x < -89.0f)
	{
		rotation.x = -89.0f;
	}

	glm::vec3 newForward;
	newForward.x = cos(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));
	newForward.y = sin(glm::radians(rotation.x));
	newForward.z = sin(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));

	forward = glm::normalize(newForward);

	right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
	up = glm::normalize(glm::cross(right, forward));

	glm::vec3 position = transformComponent->getPosition();
	position += getVelocity().z * (speed * forward) * dt;
	position += getVelocity().x * (speed * glm::cross(forward, up)) * dt;

	//glm::vec3 rotation;
	//rotation.x = pitch;
	//rotation.y = yaw;

	transformComponent->setPosition(position);
	transformComponent->setRotation(rotation);

	/*if (CameraComponent* cameraComp = entity->findComponent<CameraComponent>())
	{
		if (Camera* camera = cameraComp->getCamera())
		{
			camera->setPosition(position);
			camera->setForward(forward);
			camera->setRight(right);
			camera->setUp(up);
		}
	}*/
}

void MovementComponent::setVelocity(const glm::vec3& velocity)
{
	this->velocity = velocity;
}

const glm::vec3& MovementComponent::getVelocity() const
{
	return velocity;
}

void MovementComponent::setSpeed(float speed)
{
	this->speed = speed;
}

float MovementComponent::getSpeed() const
{
	return speed;
}

float MovementComponent::getSensivity() const
{
	return sensivity;
}

void MovementComponent::setPitchRate(float pitchRate)
{
	this->pitchRate = pitchRate * sensivity;
}

float MovementComponent::getPitchRate() const
{
	return pitchRate;
}

void MovementComponent::setYawRate(float yawRate)
{
	this->yawRate = yawRate * sensivity;
}

float MovementComponent::getYawRate() const
{
	return yawRate;
}

void MovementComponent::setMovementForward(float value)
{
	velocity.z = value;
}

void MovementComponent::setMovementSideways(float value)
{
	velocity.x = value;
}

std::unique_ptr<Component> MovementComponent::clone() const
{
	return std::make_unique<MovementComponent>(*this);
}

void MovementComponent::operator=(const MovementComponent& otherMovementComp)
{
	yawRate = otherMovementComp.yawRate;
	pitchRate = otherMovementComp.pitchRate;

	velocity = otherMovementComp.velocity;
	float speed = otherMovementComp.speed;

	forward = otherMovementComp.forward;
	up = otherMovementComp.up;
	right = otherMovementComp.right;

	sensivity = otherMovementComp.sensivity;
}

void MovementComponent::addHorizontalLook(float value)
{
	float movement = value * sensivity;

	if (transformComponent)
	{
		glm::vec3 rotation = transformComponent->getRotation();
		rotation.y = rotation.y + movement;
		// transformComp->setYaw(transformComp->getYaw() + movement);
		transformComponent->setRotation(rotation);
	}
}

void MovementComponent::addVerticalLook(float value)
{
	float movement = value * sensivity;

	if (transformComponent)
	{
		// transformComp->setPitch(transformComp->getPitch() + movement);
		glm::vec3 rotation = transformComponent->getRotation();
		rotation.x = rotation.x + movement;
		transformComponent->setRotation(rotation);
	}
}
