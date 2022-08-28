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
#include <TI/Server/Plane.h>
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
	currentRotationAngle(0.0f)
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

	if (crouchingInProgress)
	{
		updateCrouchTransition(dt);
	}

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

		if (!planeSideTransitionInProgress)
		{
			const glm::quat originalOrientation = transformComponent->getOrientation();

			// Rotate for test
			transformComponent->rotateInWorldSpace(glm::radians(90.0f), cross);

			if (physicsComponent)
			{
				const CollisionBox originalBox = physicsComponent->getCollisionBox();

				// Set a new box for test
				CollisionBox box = originalBox;
				box.orient(orientationInfo);
				physicsComponent->setCollisionBox(std::move(box));

				// Test resolve
				CollisionResult collisionResult = physicsComponent->resolveCollision(transformComponent->getPosition(), {}, dt);

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

					crouching = true;

					CollisionBox constrainedBox = physicsComponent->getCollisionBox();

					glm::vec3 s = constrainedBox.getUnorientedSize();
					glm::vec3 o = constrainedBox.getUnorientedOffset();
					s.y = 0.6f;
					o.y = 0.3f;
					constrainedBox.setSize(s);
					constrainedBox.setOffset(o);

					constrainedBox.orient(orientationInfo);

					physicsComponent->setCollisionBox(constrainedBox);
					
					if (auto cameraComponent = entity->findComponent<CameraComponent>())
					{
						// headPosition[previousOrientationInfo.heightAxis] = 0;
						crouching = true;
						cameraComponent->setPosition({});

						sideRotationAxis = cross;
						shouldRotate = true;
						previousOrientationInfo = orientationInfo;
					}

					//miniBox.setSize({ box.getUnorientedSize().x, 0.95f, box.getUnorientedSize().z });
					//miniBox.setOffset({ box.getUnorientedOffset().x, 0.425f, box.getUnorientedOffset().y});
					// miniBox.orient(previousOrientationInfo);
					//physicsComponent->setCollisionBox(miniBox);

					//glm::vec3 miniBoxPositionOffset;
					//miniBoxPositionOffset[orientationInfo.heightAxis] = miniBox.getUnorientedSize().yy;

					//drawDebugBox(transformComponent->getPosition(), miniBox.getUnorientedSize(), { 0.0f, 0.0f, 1.0f, 1.0f }, 2.0f, true);

					//physicsComponent->resolveCollision(transformComponent->getPosition(), {}, dt);

					//if (!collisionResult.collidedAxis[orientationInfo.sideAxis] &&
					//	!collisionResult.collidedAxis[orientationInfo.frontAxis] &&
					//	!collisionResult.collidedAxis[orientationInfo.heightAxis])
					//{
					//	physicsComponent->setCollisionBox(originalBox);
					//}
					//else
					//{
					//	physicsComponent->setCollisionBox(miniBox);
					//}
					//if (!crouching)
					//{
					//	toggleCrouch();
					//}

					//sideRotationAxis = cross;
					//shouldRotate = true;
					//previousOrientationInfo = orientationInfo;
				}
				
				transformComponent->setOrientation(originalOrientation);
			}
		}
	}

	if (shouldRotate)
	{
		planeSideTransitionInProgress = true;

		const float rotationAngle = 90.0f;
		const float rotationStep = 300.0f * dt;

		if (currentRotationAngle < rotationAngle)
		{
			transformComponent->rotateInWorldSpace(glm::radians(rotationStep), sideRotationAxis);
			currentRotationAngle += rotationStep;
		}
		else
		{
			const float remainingRotationStep = rotationAngle - currentRotationAngle;
			transformComponent->rotateInWorldSpace(glm::radians(remainingRotationStep), sideRotationAxis);

			currentRotationAngle = 0.0f;
			shouldRotate = false;
			planeSideTransitionInProgress = false;
		}
	}
}

void MovementComponent::updateCrouchTransition(float dt)
{
	const float step = 3.0f;

	headPosition.y += (crouching ? -step : step) * dt;

	if (crouching ? (headPosition.y < headPositionCrouching) : (headPosition.y > headPositionStanding))
	{
		headPosition.y = crouching ? headPositionCrouching : headPositionStanding;
		crouchingInProgress = false;
	}

	if (auto cameraComponent = entity->findComponent<CameraComponent>())
	{
		cameraComponent->setPosition(headPosition);
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

void MovementComponent::toggleCrouch()
{
	if (!crouchingInProgress && !planeSideTransitionInProgress && movementState == MovementState::Walk)
	{
		crouching = !crouching;
		crouchingInProgress = true;

		const float crouchSizeHeight = 0.6f;
		const float crouchOffsetHeight = 0.3f;

		if (CollisionBox box = physicsComponent->getCollisionBox();
			crouching)
		{
			box.setSize({ box.getUnorientedSize().x, crouchSizeHeight, box.getUnorientedSize().z});
			box.setOffset({ box.getUnorientedOffset().x, crouchOffsetHeight, box.getUnorientedOffset().z });
			physicsComponent->setCollisionBox(box);
		}
		else
		{
			// Test collision
			// TODO: Make a function to not take dt as a parameter!!
			box.setSize({ box.getUnorientedSize().x, 1.9f, box.getUnorientedSize().z });
			box.setOffset({ box.getUnorientedOffset().x, 0.85f, box.getUnorientedOffset().z });
			physicsComponent->setCollisionBox(box);

			glm::vec3 pos = transformComponent->getPosition();
			pos[orientationInfo.heightAxis] += orientationInfo.positive ? 0.1f : -0.1f;
			CollisionResult collisionResult = physicsComponent->resolveCollision(pos, {}, 0.006f);

			if (collisionResult.collidedAxis[orientationInfo.heightAxis])
			{
				crouching = true;
				crouchingInProgress = false;

				CollisionBox box = physicsComponent->getCollisionBox();
				box.setSize({ box.getUnorientedSize().x, crouchSizeHeight, box.getUnorientedSize().z });
				box.setOffset({ box.getUnorientedOffset().x, crouchOffsetHeight, box.getUnorientedOffset().z });
				physicsComponent->setCollisionBox(box);
			}
			else
			{
				transformComponent->setPosition(pos);
			}
		}
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

		return transformComponent->getPosition() + headPositionOriented;
	}

	return glm::vec3(0.0f);
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

	velocity = clampVectorMagnitude(velocity, crouching ? crawlMaxSpeed : walkMaxSpeed);

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
			glm::vec3 groundTestVelocity = velocity + getGravityVector() * 30.0f * dt;
			CollisionResult groundTestResult = physicsComponent->resolveCollision(position, groundTestVelocity, dt);
			if (!groundTestResult.collidedAxis[orientationInfo.heightAxis])
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
	if (!entity)
	{
		return;
	}

	bool gravityEnabled = transformComponent->getPlane()->isGravityEnabled();

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
		glm::vec3 position = transformComponent->getPosition();

		if (physicsComponent)
		{
			CollisionResult collisionResult = physicsComponent->resolveCollision(position, velocity, dt);

			if (collisionResult.collidedAxis[orientationInfo.heightAxis])
			{
				if ((velocity[orientationInfo.heightAxis] < 0.0f && orientationInfo.positive) || (velocity[orientationInfo.heightAxis] > 0.0f && !orientationInfo.positive))
				{
					// On land
					movementState = MovementState::Walk;
					velocity = clampVectorMagnitude(velocity, crouching ? crawlMaxSpeed : walkMaxSpeed);
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
