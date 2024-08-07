#include "MovementComponent.h"

#include <iostream>
#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <TI/Server/Entity.h>
#include <TI/Server/Component/CameraComponent.h>
#include <TI/Renderer/Camera.h>
#include <TI/Server/Component/TransformComponent.h>
#include <TI/Server/Component/PhysicsComponent.h>
#include <TI/Server/Component/CollisionComponent.h>
#include <TI/Server/Component/MeshComponent.h>
#include <TI/Server/Component/BlockGridGravityComponent.h>
#include <TI/Server/BlockGrid.h>
#include <TI/Server/CelestialBody.h>
#include <TI/Client/ChunkMesh.h>
#include <TI/Client/DebugInformation.h>
#include <TI/Util/Math.h>
#include <TI/Util/Utils.h>

MovementComponent::MovementComponent() :
	Component(),
	transformComponent(nullptr),
	physicsComponent(nullptr),
	headPosition(0.0f, 1.75f, 0.0f),
	headRotation(0.0f, 0.0f, 0.0f),
	headDirection(0.0f, 0.0f, 0.0f),
	yawRate(0.0f),
	pitchRate(0.0f),
	walkAcceleration(35.0f),
	walkAccelerationInAir(5.0f),
	walkMaxSpeed(3.5f),
	crawlMaxSpeed(1.0f),
	flightSpeed(25.0f),
	breakingFactor(1.0f),
	jumpVelocity(8.5f),
	flightMode(false),
	movementState(MovementState::Fall),
	primaryBody(nullptr)
{

}

static const float headPositionCrouching = 0.575f;
static const float headPositionStanding = 1.75f;

void MovementComponent::init()
{
	if (entity)
	{
		transformComponent = entity->findComponent<TransformComponent>();
		physicsComponent = entity->findComponent<CollisionComponent>();
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

void MovementComponent::setPitchRate(float pitchRate)
{
	this->pitchRate = pitchRate;
}

float MovementComponent::getPitchRate() const
{
	return pitchRate;
}

void MovementComponent::setYawRate(float yawRate)
{
	this->yawRate = yawRate;
}

float MovementComponent::getYawRate() const
{
	return yawRate;
}

void MovementComponent::setMovementForward(float value)
{
	input.z = value;
}

void MovementComponent::setMovementSideways(float value)
{
	input.x = value;
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
		if (auto blockGridGravityComponent = entity->findComponent<BlockGridGravityComponent>())
		{
			velocity += -blockGridGravityComponent->getGravityVector() * jumpVelocity;
		}
		
		movementState = MovementState::Fall;
	}
}

void MovementComponent::setFlyModeEnabled(bool flightMode)
{
	movementState = flightMode ? MovementState::Fly : MovementState::Fall;
	this->flightMode = flightMode;
	
	if (entity)
	{
		if (auto blockGridGravityComponent = entity->findComponent<BlockGridGravityComponent>())
		{
			blockGridGravityComponent->setRotationEnabled(!flightMode);
		}
	}
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

glm::vec3 MovementComponent::getHeadPosition() const
{
	if (transformComponent)
	{
		return transformComponent->getLocalPosition() + (transformComponent->getLocalOrientation() * headPosition);
	}

	return glm::vec3(0.0f);
}

void MovementComponent::setVelocity(const glm::vec3& newVelocity)
{
	velocity = newVelocity;
}

void MovementComponent::handleInput(float dt)
{
	if (!entity)
	{
		return;
	}

	headRotation.x += pitchRate * dt;
	headRotation.y += yawRate * dt;

	if (headRotation.x > 89.0f) headRotation.x = 89.0f;
	if (headRotation.x < -89.0f) headRotation.x = -89.0f;

	if (auto cameraComponent = entity->findComponent<CameraComponent>())
	{
		walkDirection = cameraComponent->getLocalForwardVector();

		glm::vec3 right = glm::cross(walkDirection, {0.0f, 1.0f, 0.0f});

		walkDirection = walkDirection * input.z;
		walkDirection += right * input.x;
		walkDirection.y = 0.0f;

		headDirection = glm::normalize(cameraComponent->getLocalForwardVector());

		cameraComponent->setRotation(headRotation);
		
		//drawDebugLine(cameraComponent->getDerivedPosition(), cameraComponent->getDerivedPosition() + cameraComponent->getLocalForwardVector(), { 0.0f, 0.0f, 1.0f, 1.0f }, 5.0f, false);
		//drawDebugLine(cameraComponent->getDerivedPosition(), cameraComponent->getDerivedPosition() + cameraComponent->getLocalRightVector(), { 1.0f, 0.0f, 0.0f, 1.0f }, 5.0f, false);
		//drawDebugLine(cameraComponent->getDerivedPosition(), cameraComponent->getDerivedPosition() + cameraComponent->getLocalUpVector(), { 0.0f, 1.0f, 0.0f, 1.0f }, 5.0f, false);

		if (walkDirection != glm::vec3(0.0f))
		{
			walkDirection = glm::normalize(transformComponent->getLocalOrientation() * walkDirection);
		}
	}
}

void MovementComponent::handleWalk(float dt)
{
	if (!entity)
	{
		return;
	}

	if (!transformComponent)
	{
		return;
	}

	BlockGrid* bg = transformComponent->getCurrentBlockGrid();
	if (!bg)
	{
		return;
	}

	auto blockGridGravityComponent = entity->findComponent<BlockGridGravityComponent>();
	if (!blockGridGravityComponent)
	{
		return;
	}

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

	glm::vec3 position = transformComponent->getLocalPosition();

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
		glm::vec3 groundTestVelocity = velocity + blockGridGravityComponent->getGravityVector() * 30.0f * dt;
		CollisionResult groundTestResult = physicsComponent->resolveCollision(position, groundTestVelocity, dt);

		glm::vec3 upVector = bg->getSideNormal(position);
		size_t heightAxis = 0;
		for (size_t i = 0; i < 3; ++i)
		{
			if (upVector[i] != 0.0f)
			{
				heightAxis = i;
				break;
			}
		}

		if (!groundTestResult.collidedAxis[heightAxis])
		{
			movementState = MovementState::Fall;
		}
	}

	transformComponent->setPosition(position);
	transformComponent->offset(velocity * dt);
}

void MovementComponent::handleFall(float dt)
{
	if (!entity)
	{
		return;
	}

	if (!transformComponent)
	{
		return;
	}

	BlockGrid* bg = transformComponent->getCurrentBlockGrid();
	if (!bg)
	{
		return;
	}

	auto blockGridGravityComponent = entity->findComponent<BlockGridGravityComponent>();
	if (!blockGridGravityComponent)
	{
		return;
	}

	bool gravityEnabled = transformComponent->getCurrentBlockGrid()->isGravityEnabled();

	// In air control
	if (walkDirection != glm::vec3(0.0f))
	{
		velocity += walkDirection * walkAccelerationInAir * dt;
	}

	// Apply gravity
	if (gravityEnabled)
	{
		velocity += blockGridGravityComponent->getGravityVector() * 30.0f * dt;
	}

	glm::vec3 position = transformComponent->getLocalPosition();

	if (physicsComponent)
	{
		CollisionResult collisionResult = physicsComponent->resolveCollision(position, velocity, dt);

		glm::vec3 upVector = bg->getSideNormal(position);
		size_t heightAxis = 0;
		for (size_t i = 0; i < 3; ++i)
		{
			if (upVector[i] != 0.0f)
			{
				heightAxis = i;
				break;
			}
		}

		if (collisionResult.collidedAxis[heightAxis])
		{
			if ((velocity[heightAxis] < 0.0f && upVector[heightAxis] > 0.0f) || (velocity[heightAxis] > 0.0f && upVector[heightAxis] < 0.0f))
			{
				// On land
				movementState = MovementState::Walk;
				velocity = clampVectorMagnitude(velocity, walkMaxSpeed);
			}
		}

		position = collisionResult.adjustedPosition;
		velocity = collisionResult.adjustedVelocity;
	}

	transformComponent->setPosition(position);
	transformComponent->offset(velocity * dt);
}

void MovementComponent::handleFlight(float dt)
{
	if (!entity)
	{
		return;
	}

	if (auto cameraComp = entity->findComponent<CameraComponent>())
	{
		glm::vec3 right = cameraComp->getLocalRightVector();

		glm::vec3 flightDirection = headDirection * input.z;
		flightDirection += right * input.x;

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
			//if (physicsComponent)
			//{
			//	CollisionResult collisionResult = physicsComponent->resolveCollision(position, velocity, dt);
			//	velocity = collisionResult.adjustedVelocity;
			//	position = collisionResult.adjustedPosition;
			//}

			// CoordinateSystem::Interplanetary is a hack. Originally it was CoordinateSystem::Planetary, but we lose it
			// because we may leave the planetary coordinate system
			transformComponent->offset((transformComponent->getLocalOrientation() * velocity) * dt);
		}
	}
}

void MovementComponent::addHorizontalLook(float value)
{
	headRotation.y = headRotation.y - value;
}

void MovementComponent::addVerticalLook(float value)
{
	headRotation.x = headRotation.x - value;
}
