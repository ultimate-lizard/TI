#include "MovementComponent.h"

#include <iostream>

#include <TI/Server/Entity.h>
#include <TI/Server/Component/CameraComponent.h>
#include <TI/Renderer/Camera.h>
#include <TI/Server/Component/TransformComponent.h>
#include <TI/Server/Component/PhysicsComponent.h>
#include <TI/Server/Plane.h>
#include <TI/Client/ChunkMesh.h>
#include <TI/Client/DebugInformation.h>

float maxVelocity = 100.0f;

MovementComponent::MovementComponent() :
	transformComponent(nullptr),
	physicsComponent(nullptr),
	yawRate(0.0f),
	pitchRate(0.0f),
	speed(50.0f),
	forward(0.0f, 0.0f, -1.0f),
	sensivity(20.0f),
	flyMode(true)
{
}

MovementComponent::MovementComponent(const MovementComponent& otherMovementComp) :
	Component(otherMovementComp),
	transformComponent(otherMovementComp.transformComponent),
	physicsComponent(otherMovementComp.physicsComponent),
	yawRate(otherMovementComp.yawRate),
	pitchRate(otherMovementComp.pitchRate),
	movementDirection(otherMovementComp.movementDirection),
	speed(otherMovementComp.speed),
	forward(otherMovementComp.forward),
	sensivity(otherMovementComp.sensivity),
	flyMode(otherMovementComp.flyMode)
{
}

void MovementComponent::init()
{
	if (entity)
	{
		transformComponent = entity->findComponent<TransformComponent>();
		physicsComponent = entity->findComponent<PhysicsComponent>();
	}
}

void MovementComponent::tick(float dt)
{
	if (transformComponent && physicsComponent)
	{
		glm::vec3 rotation = transformComponent->getRotation();

		rotation.x += pitchRate * dt;
		rotation.y += yawRate * dt;

		if (rotation.x > 89.0f) rotation.x = 89.0f;
		if (rotation.x < -89.0f) rotation.x = -89.0f;

		forward.x = cos(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));
		forward.y = 0.0f;
		forward.z = sin(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));

		if (flyMode)
		{
			forward.y = sin(glm::radians(rotation.x));
			forward = glm::normalize(forward);
		}

		glm::vec3 movementForward = forward;
		movementForward = glm::normalize(movementForward);

		if (!flyMode)
		{
			forward.y = sin(glm::radians(rotation.x));
			forward = glm::normalize(forward);
		}

		glm::vec3 right = glm::normalize(glm::cross(movementForward, glm::vec3(0.0f, 1.0f, 0.0f)));
		glm::vec3 up = glm::normalize(glm::cross(right, movementForward));

		glm::vec3 velocity = movementDirection.z * (speed * movementForward);
		velocity += movementDirection.x * (speed * glm::cross(movementForward, up));

		glm::vec3 position = transformComponent->getPosition();

		transformComponent->setRotation(rotation);

		physicsComponent->setAbsoluteVelocity(velocity);
	}
}

void MovementComponent::setVelocity(const glm::vec3& velocity)
{
	this->movementDirection = velocity;
}

const glm::vec3& MovementComponent::getVelocity() const
{
	return movementDirection;
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
	movementDirection.z = value;
}

void MovementComponent::setMovementSideways(float value)
{
	movementDirection.x = value;
}

void MovementComponent::jump()
{
	if (physicsComponent)
	{
		if (physicsComponent->isOnGround())
		{
			physicsComponent->addVelocity({ 0.0f, 65.0f, 0.0f });
		}
	}
}

void MovementComponent::setFlyModeEnabled(bool flyMode)
{
	this->flyMode = flyMode;
	if (physicsComponent)
	{
		glm::vec3 velocity = physicsComponent->getVelocity();
		velocity.y = 0.0f;
		physicsComponent->setVelocity(velocity);
	}
}

bool MovementComponent::isFlyModeEnabled() const
{
	return flyMode;
}

std::unique_ptr<Component> MovementComponent::clone() const
{
	return std::make_unique<MovementComponent>(*this);
}

const glm::vec3& MovementComponent::getForward() const
{
	return forward;
}

void MovementComponent::addHorizontalLook(float value)
{
	float movement = value * sensivity;

	if (transformComponent)
	{
		glm::vec3 rotation = transformComponent->getRotation();
		rotation.y = rotation.y + movement;
		transformComponent->setRotation(rotation);
	}
}

void MovementComponent::addVerticalLook(float value)
{
	float movement = value * sensivity;

	if (transformComponent)
	{
		glm::vec3 rotation = transformComponent->getRotation();
		rotation.x = rotation.x + movement;
		transformComponent->setRotation(rotation);
	}
}
