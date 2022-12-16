#include "BlockGridGravityComponent.h"

#include <TI/Server/Entity.h>
#include <TI/Server/BlockGrid.h>
#include <TI/Server/Component/TransformComponent.h>
#include <TI/Server/Component/CollisionComponent.h>
#include <TI/Util/Utils.h>

BlockGridGravityComponent::BlockGridGravityComponent() :
	Component(),
	pendingRotationAngle(0.0f),
	shouldRotate(false),
	planeSideTransitionInProgress(false),
	currentRotationAngle(0.0f),
	rotationEnabled(true)
{

}

void BlockGridGravityComponent::tick(float dt)
{
	updateSideRotation(dt);
}

std::unique_ptr<Component> BlockGridGravityComponent::clone() const
{
	return std::make_unique<BlockGridGravityComponent>(*this);
}

glm::vec3 BlockGridGravityComponent::getGravityVector() const
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

void BlockGridGravityComponent::setRotationEnabled(bool enabled)
{
	rotationEnabled = enabled;
}

bool BlockGridGravityComponent::isRotationEnabled() const
{
	return rotationEnabled;
}

void BlockGridGravityComponent::updateSideRotation(float dt)
{
	if (!isRotationEnabled())
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

	const glm::vec3 currentUpVector = bg->getSideNormal(transformComponent->getDerivedPosition(false));
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

		// Rotate collision box
		if (!planeSideTransitionInProgress) //&& !shouldEscape)
		{
			const glm::quat originalOrientation = transformComponent->getLocalOrientation();

			// Rotate for test
			transformComponent->rotateInWorldSpace(pendingRotationAngle, cross);

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
