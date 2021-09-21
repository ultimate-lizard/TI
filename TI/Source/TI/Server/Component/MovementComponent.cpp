#include "MovementComponent.h"

#include <iostream>
#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>

#include <TI/Server/Entity.h>
#include <TI/Server/Component/CameraComponent.h>
#include <TI/Renderer/Camera.h>
#include <TI/Server/Component/TransformComponent.h>
#include <TI/Server/Component/PhysicsComponent.h>
#include <TI/Server/Plane.h>
#include <TI/Client/ChunkMesh.h>
#include <TI/Client/DebugInformation.h>

MovementComponent::MovementComponent() :
	transformComponent(nullptr),
	physicsComponent(nullptr),
	headForward(0.0f, 0.0f, 1.0f),
	yawRate(0.0f),
	pitchRate(0.0f),
	onGroundSpeed(45.0f),
	inAirSpeed(35.0f),
	jumpVelocity({0.0f, 90.0f, 0.0f}),
	lookSensivity(20.0f),
	flightMode(false),
	wasOnGround(false)
{
}

MovementComponent::MovementComponent(const MovementComponent& other) :
	Component(other),
	transformComponent(other.transformComponent),
	physicsComponent(other.physicsComponent),
	headRotation(other.headRotation),
	headForward(other.headForward),
	movementDirection(other.movementDirection),
	yawRate(other.yawRate),
	pitchRate(other.pitchRate),
	onGroundSpeed(other.onGroundSpeed),
	inAirSpeed(other.inAirSpeed),
	jumpVelocity(other.jumpVelocity),
	lookSensivity(other.lookSensivity),
	flightMode(other.flightMode),
	wasOnGround(other.wasOnGround)
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
		headRotation.x += pitchRate * dt;
		headRotation.y += yawRate * dt;

		if (headRotation.x > 89.0f) headRotation.x = 89.0f;
		if (headRotation.x < -89.0f) headRotation.x = -89.0f;

		glm::vec3 newForward;
		newForward.x = cos(glm::radians(headRotation.y)) * cos(glm::radians(headRotation.x));
		newForward.y = sin(glm::radians(headRotation.x));
		newForward.z = sin(glm::radians(headRotation.y)) * cos(glm::radians(headRotation.x));

		headForward = glm::normalize(newForward);

		newForward.y = 0.0f; // We don't need to calculate Y axis during player movement
		newForward = glm::normalize(newForward);

		glm::vec3 right = glm::normalize(glm::cross(newForward, glm::vec3(0.0f, 1.0f, 0.0f)));
		glm::vec3 up = glm::normalize(glm::cross(right, newForward));

		glm::vec3 velocityGoal = newForward * movementDirection.x;
		velocityGoal += right * movementDirection.z;

		transformComponent->setRotation(headRotation);

		if (movementDirection.x || movementDirection.z)
		{
			velocityGoal = glm::normalize(velocityGoal);
		}

		glm::vec3 velocity = velocityGoal;
		velocity *= onGroundSpeed;

		// Velocity penalty when landing
		if (!wasOnGround && physicsComponent->isOnGround())
		{
			// physicsComponent->addVelocity(physicsComponent->getVelocity() * -1.0f);
		}

		wasOnGround = physicsComponent->isOnGround();

		if (!flightMode)
		{
			if (physicsComponent->isOnGround())
			{
				velocity = glm::clamp(velocity, glm::vec3(-onGroundSpeed), glm::vec3(onGroundSpeed));
				physicsComponent->addVelocity(velocity * dt * 100.f);
			}
			else
			{
				velocity = glm::clamp(velocity, glm::vec3(-inAirSpeed), glm::vec3(inAirSpeed));
				physicsComponent->addVelocity(velocity * dt * 10.0f);
			}
		}
		else
		{
			// Flight mode movement calculation
			glm::vec3 velocityGoal = headForward * movementDirection.x;
			velocityGoal += right * movementDirection.z;

			if (movementDirection.x || movementDirection.z)
			{
				velocityGoal = glm::normalize(velocityGoal);
			}

			const float FLIGHT_SPEED = onGroundSpeed * 3.0f;
			velocity = velocityGoal * FLIGHT_SPEED;
			velocity = glm::clamp(velocity, glm::vec3(-FLIGHT_SPEED), glm::vec3(FLIGHT_SPEED));
			physicsComponent->setVelocity(velocity);
		}
	}
}

std::unique_ptr<Component> MovementComponent::clone() const
{
	return std::make_unique<MovementComponent>(*this);
}

void MovementComponent::setVelocity(const glm::vec3& velocity)
{
	this->movementDirection = velocity;
}

const glm::vec3& MovementComponent::getVelocity() const
{
	return movementDirection;
}

void MovementComponent::setOnGroundSpeed(float speed)
{
	this->onGroundSpeed = speed;
}

float MovementComponent::getOnGroundSpeed() const
{
	return onGroundSpeed;
}

float MovementComponent::getSensivity() const
{
	return lookSensivity;
}

void MovementComponent::setPitchRate(float pitchRate)
{
	this->pitchRate = pitchRate * lookSensivity;
}

float MovementComponent::getPitchRate() const
{
	return pitchRate;
}

void MovementComponent::setYawRate(float yawRate)
{
	this->yawRate = yawRate * lookSensivity;
}

float MovementComponent::getYawRate() const
{
	return yawRate;
}

void MovementComponent::setMovementForward(float value)
{
	movementDirection.x = value;
}

void MovementComponent::setMovementSideways(float value)
{
	movementDirection.z = value;
}

void MovementComponent::jump()
{
	if (physicsComponent)
	{
		if (physicsComponent->isOnGround())
		{
			physicsComponent->addVelocity(jumpVelocity);
		}
	}
}

void MovementComponent::setFlyModeEnabled(bool flyMode)
{
	this->flightMode = flyMode;
	if (physicsComponent)
	{
		glm::vec3 velocity = physicsComponent->getVelocity();
		velocity.y = 0.0f;
		physicsComponent->setVelocity(velocity);
	}
}

bool MovementComponent::isFlyModeEnabled() const
{
	return flightMode;
}

const glm::vec3& MovementComponent::getHeadForward() const
{
	return headForward;
}

const glm::vec3& MovementComponent::getHeadRotation() const
{
	return headRotation;
}

void MovementComponent::addHorizontalLook(float value)
{
	float movement = value * lookSensivity;
	headRotation.y = headRotation.y + movement;
}

void MovementComponent::addVerticalLook(float value)
{
	float movement = value * lookSensivity;
	headRotation.x = headRotation.x + movement;
}
