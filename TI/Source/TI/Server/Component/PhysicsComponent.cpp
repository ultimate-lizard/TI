#include "PhysicsComponent.h"

#include <TI/Server/Entity.h>
#include <TI/Server/Component/TransformComponent.h>
#include <TI/Server/Component/CollisionComponent.h>
#include <TI/Server/BlockGrid.h>
#include <TI/Util/Math.h>
#include <TI/Util/Utils.h>

PhysicsComponent::PhysicsComponent() :
	Component(),
	pendingRotationAngle(0.0f),
	shouldRotate(false),
	planeSideTransitionInProgress(false),
	currentRotationAngle(0.0f),
	friction(0.95f)
{

}

void PhysicsComponent::tick(float dt)
{
	updatePlaneSideRotation(dt);

	if (!entity)
	{
		return;
	}

	auto transformComponent = entity->findComponent<TransformComponent>();
	if (!transformComponent)
	{
		return;
	}

	BlockGrid* bg = transformComponent->getCurrentBlockGrid();
	if (!bg)
	{
		return;
	}

	bool gravityEnabled = transformComponent->getCurrentBlockGrid()->isGravityEnabled();

	// Apply gravity
	if (gravityEnabled)
	{
		velocity += getGravityVector() * 30.0f * dt;
	}

	glm::vec3 position = transformComponent->getLocalPosition();

	auto collisionComponent = entity->findComponent<CollisionComponent>();
	if (collisionComponent)
	{
		CollisionResult collisionResult = collisionComponent->resolveCollision(position, velocity, dt);
		//collisionResult.collidedAxis = transformComponent->getLocalOrientation() * utils::alightVectorToAxis(glm::vec3(collisionResult.collidedAxis));

		//glm::vec3 upVector = transformComponent->getLocalOrientation() * bg->getSideNormal(position);

		position = collisionResult.adjustedPosition;
		velocity = collisionResult.adjustedVelocity;

		if (collisionResult.collidedAxis != glm::bvec3(0.0f))
		{
			//glm::vec3 orientedVelocity = transformComponent->getLocalOrientation() * velocity;
			//if ((orientedVelocity.y < 0.0f && upVector.y > 0.0f) || (orientedVelocity.y > 0.0f && upVector.y < 0.0f))
			//{
			//	// On land
			//	/*movementState = MovementState::Walk;
			//	velocity = clampVectorMagnitude(velocity, walkMaxSpeed);*/
			//}
			velocity *= friction;
		}
	}

	transformComponent->setLocalPosition(position);
	transformComponent->offset(velocity * dt);
}

void PhysicsComponent::updatePlaneSideRotation(float dt)
{
	auto transformComponent = entity->findComponent<TransformComponent>();
	if (!transformComponent)
	{
		return;
	}

	BlockGrid* bg = transformComponent->getCurrentBlockGrid();
	if (!bg)
	{
		return;
	}

	const glm::vec3 currentUpVector = bg->getSideNormal(transformComponent->getDerivedPosition());
	if (previousUpVector == glm::vec3(0.0f))
	{
		previousUpVector = currentUpVector;
	}

	// This check is only for not executing this logic every frame
	if (previousUpVector != currentUpVector)
	{
		glm::vec3 cross = glm::cross(previousUpVector, currentUpVector);
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

		// Rotate collision box
		if (!planeSideTransitionInProgress) //&& !shouldEscape)
		{
			const glm::quat originalOrientation = transformComponent->getLocalOrientation();

			// Rotate for test
			transformComponent->rotateInWorldSpaceExclusive(pendingRotationAngle, cross);

			if (auto collisionComponent = entity->findComponent<CollisionComponent>())
			{
				const CollisionBox originalBox = collisionComponent->getCollisionBox();

				// Set a new box for test
				CollisionBox box = originalBox;

				box.orient(currentUpVector, utils::alightVectorToAxis(transformComponent->getForwardVector()), utils::alightVectorToAxis(glm::cross(transformComponent->getForwardVector(), currentUpVector)));

				collisionComponent->setCollisionBox(std::move(box));

				// Test resolve
				CollisionResult collisionResult = collisionComponent->resolveCollision(transformComponent->getLocalPosition(), {}, dt);

				// TODO: Add another collision box to prevent colliding camera with world during side transition
				if (collisionResult.collidedAxis == glm::bvec3(false))
				{
					// Didn't collide with anything. Test successful
					sideRotationAxis = cross;
					shouldRotate = true;
					previousUpVector = currentUpVector;
				}
				else
				{
					collisionComponent->setCollisionBox(originalBox);
				}

				transformComponent->setLocalOrientation(originalOrientation);
			}
		}
	}

	if (shouldRotate)
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
			transformComponent->rotate(remainingRotationStep, sideRotationAxis);

			currentRotationAngle = 0.0f;
			shouldRotate = false;
			planeSideTransitionInProgress = false;
		}
	}
}

void PhysicsComponent::addForce(const glm::vec3& force)
{
	velocity += force;
}

std::unique_ptr<Component> PhysicsComponent::clone() const
{
	return std::make_unique<PhysicsComponent>(*this);
}

glm::vec3 PhysicsComponent::getGravityVector() const
{
	if (!entity)
	{
		return {};
	}

	auto transformComponent = entity->findComponent<TransformComponent>();
	if (!transformComponent)
	{
		return {};
	}

	BlockGrid* bg = transformComponent->getCurrentBlockGrid();
	if (!bg)
	{
		return {};
	}

	glm::vec3 result = -bg->getSideNormal(transformComponent->getDerivedPosition());

	return result;
}