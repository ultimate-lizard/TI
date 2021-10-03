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
#include <TI/Util/Math.h>

MovementComponent::MovementComponent() :
	Component(),
	transformComponent(nullptr),
	physicsComponent(nullptr),
	headDirection(0.0f, 0.0f, 1.0f),
	yawRate(0.0f),
	pitchRate(0.0f),
	walkAcceleration(35.0f),
	walkAccelerationInAir(5.0f),
	walkMaxSpeed(3.5f),
	flightSpeed(25.0f),
	breakingFactor(1.0f),
	jumpVelocity({0.0f, 11.2f, 0.0f}),
	lookSensivity(20.0f),
	flightMode(false),
	movementState(MovementState::Fall)
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
	handleInput(dt);

	switch (movementState)
	{
	case MovementState::Walk:
		handleWalk(dt);
		break;
	case MovementState::Fall:
		handleFall(dt);
		break;
	case MovementState::Fly:
		handleFlight(dt);
		break;
	}
}

std::unique_ptr<Component> MovementComponent::clone() const
{
	return std::make_unique<MovementComponent>(*this);
}

float MovementComponent::getLookSensivity() const
{
	return lookSensivity;
}

void MovementComponent::setLookSensivity(float sensivity)
{
	this->lookSensivity = sensivity;
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
	input.x = value;
}

void MovementComponent::setMovementSideways(float value)
{
	input.z = value;
}

void MovementComponent::setWalkAcceleration(float acceleration)
{
	this->walkAcceleration = acceleration;
}

float MovementComponent::getWalkAcceleration() const
{
	return walkAcceleration;
}

void MovementComponent::setWalkAccelerationInAir(float acceleration)
{
	this->walkAccelerationInAir = acceleration;
}

float MovementComponent::getWalkAccelerationInAir() const
{
	return walkAccelerationInAir;
}

void MovementComponent::setWalkMaxSpeed(float maxSpeed)
{
	this->walkMaxSpeed = maxSpeed;
}

float MovementComponent::getWalkMaxSpeed() const
{
	return walkMaxSpeed;
}

void MovementComponent::setFlightSpeed(float flightSpeed)
{
	this->flightSpeed = flightSpeed;
}

float MovementComponent::getFlightSpeed() const
{
	return flightSpeed;
}

void MovementComponent::setBreakingFactor(float breakingFactor)
{
	this->breakingFactor = breakingFactor;
}

float MovementComponent::getBreakingFactor() const
{
	return breakingFactor;
}

void MovementComponent::jump()
{
	if (movementState == MovementState::Walk)
	{
		velocity += jumpVelocity;
		movementState = MovementState::Fall;
	}
}

void MovementComponent::setFlyModeEnabled(bool flightMode)
{
	movementState = flightMode ? MovementState::Fly : MovementState::Fall;
	this->flightMode = flightMode;
}

bool MovementComponent::isFlightModeEnabled() const
{
	return flightMode;
}

const glm::vec3& MovementComponent::getHeadDirection() const
{
	return headDirection;
}

const glm::vec3& MovementComponent::getHeadRotation() const
{
	return headRotation;
}

void MovementComponent::handleInput(float dt)
{
	headRotation.x += pitchRate * dt;
	headRotation.y += yawRate * dt;

	if (headRotation.x > 89.0f) headRotation.x = 89.0f;
	if (headRotation.x < -89.0f) headRotation.x = -89.0f;

	glm::vec3 newForward;
	newForward.x = cos(glm::radians(headRotation.y)) * cos(glm::radians(headRotation.x));
	newForward.y = sin(glm::radians(headRotation.x));
	newForward.z = sin(glm::radians(headRotation.y)) * cos(glm::radians(headRotation.x));

	headDirection = glm::normalize(newForward);

	newForward.y = 0.0f; // We don't need to calculate Y axis during player movement
	walkDirection = glm::normalize(newForward);

	glm::vec3 right = glm::normalize(glm::cross(walkDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
	glm::vec3 up = glm::normalize(glm::cross(right, walkDirection));

	walkDirection = walkDirection * input.x;
	walkDirection += right * input.z;
	if (walkDirection != glm::vec3(0.0f))
	{
		walkDirection = glm::normalize(walkDirection);
	}

	transformComponent->setRotation(headRotation);
}

void MovementComponent::handleWalk(float dt)
{
	// Acceleration
	if (walkDirection != glm::vec3(0.0f))
	{
		velocity += walkDirection * walkAcceleration * dt;
	}
	// Deceleration
	else if (velocity != glm::vec3(0.0f))
	{
		glm::vec3 nextAcceleration = -glm::normalize(velocity) * walkAcceleration * breakingFactor * dt;
		if (glm::dot(velocity + nextAcceleration, velocity) >= 0.0f)
		{
			velocity += -glm::normalize(velocity) * walkAcceleration * breakingFactor * dt;
		}
		else
		{
			velocity = glm::vec3(0.0f);
		}
	}

	velocity = clampVectorMagnitude(velocity, walkMaxSpeed);

	if (transformComponent)
	{
		glm::vec3 position = transformComponent->getPosition();

		// Apply collisions
		if (physicsComponent)
		{
			if (velocity != glm::vec3(0.0f))
			{
				CollisionResult result = physicsComponent->resolveCollision(position, velocity, dt);

				velocity = result.adjustedVelocity;
				position = result.adjustedPosition;
			}

			// Check if no longer on ground
			glm::vec3 groundTestVelocity = velocity + glm::vec3(0.0f, -1.0f, 0.0f) * 30.0f * dt;
			CollisionResult groundTestResult = physicsComponent->resolveCollision(position, groundTestVelocity, dt);
			if (!groundTestResult.collidedAxis.y)
			{
				movementState = MovementState::Fall;
			}
		}

		position += velocity * dt;
		transformComponent->setPosition(position);
	}
}

void MovementComponent::handleFall(float dt)
{
	bool gravityEnabled = transformComponent->getPlane()->isGravityEnabled();

	// In air control
	if (walkDirection != glm::vec3(0.0f))
	{
		velocity += walkDirection * walkAccelerationInAir * dt;
	}

	// Apply gravity
	if (gravityEnabled)
	{
		velocity += glm::vec3(0.0f, -1.0f, 0.0f) * 30.0f * dt;
	}

	if (transformComponent)
	{
		glm::vec3 position = transformComponent->getPosition();

		if (physicsComponent)
		{
			CollisionResult collisionResult = physicsComponent->resolveCollision(position, velocity, dt);

			if (collisionResult.collidedAxis.y)
			{
				if (velocity.y < 0.0f) // Should take in consideration gravity direction (this might not be just Y axis)
				{
					// On land
					movementState = MovementState::Walk;
					velocity = clampVectorMagnitude(velocity, walkMaxSpeed);
				}
			}

			position = collisionResult.adjustedPosition;
			velocity = collisionResult.adjustedVelocity;
		}

		position += velocity * dt;
		transformComponent->setPosition(position);
	}
}

void MovementComponent::handleFlight(float dt)
{
	glm::vec3 right = glm::normalize(glm::cross(headDirection, glm::vec3(0.0f, 1.0f, 0.0f)));

	glm::vec3 flightDirection = headDirection * input.x;
	flightDirection += right * input.z;

	if (flightDirection != glm::vec3(0.0f))
	{
		velocity = flightDirection * flightSpeed;
	}
	else
	{
		velocity = glm::vec3(0.0f);
	}

	if (transformComponent)
	{
		glm::vec3 position = transformComponent->getPosition();

		if (physicsComponent)
		{
			CollisionResult collisionResult = physicsComponent->resolveCollision(position, velocity, dt);
			velocity = collisionResult.adjustedVelocity;
			position = collisionResult.adjustedPosition;
		}

		position += velocity * dt;
		transformComponent->setPosition(position);
	}
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
