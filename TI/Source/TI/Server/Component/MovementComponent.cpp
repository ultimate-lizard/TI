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
#include <TI/Server/Component/MeshComponent.h>
#include <TI/Server/BlockGrid.h>
#include <TI/Server/CelestialBody.h>
#include <TI/Client/ChunkMesh.h>
#include <TI/Client/DebugInformation.h>
#include <TI/Util/Math.h>

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
	shouldRotate(false),
	planeSideTransitionInProgress(false),
	currentRotationAngle(0.0f),
	pendingRotationAngle(90.0f),
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
		physicsComponent = entity->findComponent<PhysicsComponent>();
	}
}

void MovementComponent::tick(float dt)
{
	handleInput(dt);

	updatePlaneSideRotation(dt);

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

void MovementComponent::updatePlaneSideRotation(float dt)
{
	if (!transformComponent)
	{
		return;
	}

	if (std::optional<OrientationInfo> orientation = transformComponent->getOrientationInfo(); orientation.has_value())
	{
		orientationInfo = orientation.value();
	}

	if (orientationInfo != previousOrientationInfo)
	{
		glm::vec3 previousHeightVector;
		previousHeightVector[previousOrientationInfo.heightAxis] = previousOrientationInfo.positive ? 1 : -1;
		glm::vec3 currentHeightVector;
		currentHeightVector[orientationInfo.heightAxis] = orientationInfo.positive ? 1 : -1;

		glm::vec3 cross = glm::cross(previousHeightVector, currentHeightVector);
		if (cross == glm::vec3(0.0f))
		{
			// Find a negative 0 from the cross product 0, 0, 0. Axis with negative 0 is the correct rotation axis
			// TODO: WARNING! Find better soultion. Might be different result with a different compilator?
			for (glm::vec3::length_type i = 0; i < 3; ++i)
			{
				if (std::signbit(cross[i]))
				{
					cross[i] = 1.0f;
					break;
				}
			}

			pendingRotationAngle = 180.0f;
			
		}
		else
		{
			pendingRotationAngle = 90.0f;
		}

		//bool shouldEscape = false;
		//if (transformComponent)
		//{
		//	if (BlockGrid* bg = transformComponent->getCurrentBlockGrid())
		//	{
		//		// Assume block grid are always cubical
		//		const float bgSizeInBlocks = bg->getBlockGridSize().x * bg->getChunkSize();
		//		const float distanceToBgCenter = glm::distance(transformComponent->getLocalPosition(), glm::vec3(bgSizeInBlocks / 2.0f));
		//		const float escapeAltitude = bgSizeInBlocks;
		//		if (distanceToBgCenter > escapeAltitude)
		//		{
		//			shouldEscape = true;
		//		}
		//	}
		//}

		if (!planeSideTransitionInProgress && !flightMode) //&& !shouldEscape)
		{
			const glm::quat originalOrientation = transformComponent->getLocalOrientation();

			// Rotate for test
			transformComponent->rotateInWorldSpaceExclusive(pendingRotationAngle, cross);

			if (physicsComponent)
			{
				const CollisionBox originalBox = physicsComponent->getCollisionBox();

				// Set a new box for test
				CollisionBox box = originalBox;
				box.orient(orientationInfo);
				physicsComponent->setCollisionBox(std::move(box));

				// Test resolve
				CollisionResult collisionResult = physicsComponent->resolveCollision(transformComponent->getLocalPosition(), {}, dt);

				// TODO: Add another collision box to prevent colliding camera with world during side transition
				if (!collisionResult.collidedAxis[orientationInfo.sideAxis] &&
					!collisionResult.collidedAxis[orientationInfo.frontAxis] &&
					!collisionResult.collidedAxis[orientationInfo.heightAxis])
				{
					// Didn't collide with anything. Test successful
					sideRotationAxis = cross;
					shouldRotate = true;
					previousOrientationInfo = orientationInfo;
				}
				else
				{
					physicsComponent->setCollisionBox(originalBox);
				}
				
				transformComponent->setLocalOrientation(originalOrientation);
			}
		}
	}

	if (shouldRotate && !flightMode)
	{
		planeSideTransitionInProgress = true;

		const float rotationStep = 300.0f * dt;

		if (currentRotationAngle < pendingRotationAngle)
		{
			transformComponent->rotate(rotationStep, sideRotationAxis);
			currentRotationAngle += rotationStep;
		}
		else
		{
			const float remainingRotationStep = pendingRotationAngle - currentRotationAngle;
			transformComponent->rotate(remainingRotationStep, sideRotationAxis, CoordinateSystem::Planetary);

			currentRotationAngle = 0.0f;
			shouldRotate = false;
			planeSideTransitionInProgress = false;
		}
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
		velocity += -getGravityVector() * jumpVelocity;
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

glm::vec3 MovementComponent::getHeadPosition() const
{
	if (transformComponent)
	{
		glm::vec3 headPositionOriented;
		headPositionOriented[orientationInfo.heightAxis] = headPosition.y;

		if (!orientationInfo.positive)
		{
			headPositionOriented *= -1.0f;
		}

		return transformComponent->getLocalPosition() + headPositionOriented;
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
		walkDirection = glm::normalize(cameraComponent->getForwardVector());

		glm::vec3 up;
		up[orientationInfo.heightAxis] = orientationInfo.positive ? 1 : -1;
		glm::vec3 right = glm::cross(walkDirection, up);

		walkDirection = walkDirection * input.z;
		walkDirection += right * input.x;
		walkDirection[orientationInfo.heightAxis] = 0.0f;

		headDirection = cameraComponent->getForwardVector();

		cameraComponent->setRotation(headRotation);
	}
	
	if (walkDirection != glm::vec3(0.0f))
	{
		walkDirection = glm::normalize(walkDirection);
	}
}

void MovementComponent::handleWalk(float dt)
{
	if (!entity)
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

	if (transformComponent)
	{
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
			glm::vec3 groundTestVelocity = velocity + getGravityVector() * 30.0f * dt;
			CollisionResult groundTestResult = physicsComponent->resolveCollision(position, groundTestVelocity, dt);
			if (!groundTestResult.collidedAxis[orientationInfo.heightAxis])
			{
				movementState = MovementState::Fall;
			}
		}

		transformComponent->setLocalPosition(position);
		transformComponent->offset(velocity * dt);
	}
}

void MovementComponent::handleFall(float dt)
{
	if (!entity)
	{
		return;
	}

	if (!transformComponent->getCurrentBlockGrid())
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
		velocity += getGravityVector() * 30.0f * dt;
	}

	if (transformComponent)
	{
		glm::vec3 position = transformComponent->getLocalPosition();

		if (physicsComponent)
		{
			CollisionResult collisionResult = physicsComponent->resolveCollision(position, velocity, dt);

			if (collisionResult.collidedAxis[orientationInfo.heightAxis])
			{
				if ((velocity[orientationInfo.heightAxis] < 0.0f && orientationInfo.positive) || (velocity[orientationInfo.heightAxis] > 0.0f && !orientationInfo.positive))
				{
					// On land
					movementState = MovementState::Walk;
					velocity = clampVectorMagnitude(velocity, walkMaxSpeed);
				}
			}

			position = collisionResult.adjustedPosition;
			velocity = collisionResult.adjustedVelocity;
		}

		transformComponent->setLocalPosition(position);
		transformComponent->offset(velocity * dt);
	}
}

void MovementComponent::handleFlight(float dt)
{
	if (!entity)
	{
		return;
	}

	if (auto cameraComp = entity->findComponent<CameraComponent>())
	{
		glm::vec3 right = cameraComp->getRightVector();

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

			transformComponent->offset(velocity * dt);
		}
	}
}

glm::vec3 MovementComponent::getGravityVector() const
{
	glm::vec3 gravityVector;
	gravityVector[orientationInfo.heightAxis] = orientationInfo.positive ? -1 : 1;
	
	return gravityVector;
}

void MovementComponent::addHorizontalLook(float value)
{
	headRotation.y = headRotation.y - value;
}

void MovementComponent::addVerticalLook(float value)
{
	headRotation.x = headRotation.x - value;
}
