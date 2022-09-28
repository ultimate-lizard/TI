#include "PhysicsComponent.h"

#include <TI/Server/Entity.h>
#include <TI/Server/Component/TransformComponent.h>
#include <TI/Server/Component/CollisionComponent.h>
#include <TI/Server/Component/BlockGridGravityComponent.h>
#include <TI/Server/BlockGrid.h>
#include <TI/Util/Math.h>
#include <TI/Util/Utils.h>

PhysicsComponent::PhysicsComponent() :
	Component(),
	friction(0.95f)
{

}

void PhysicsComponent::tick(float dt)
{
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
		if (auto blockGridGravityComponent = entity->findComponent<BlockGridGravityComponent>())
		{
			velocity += blockGridGravityComponent->getGravityVector() * 30.0f * dt;
		}
	}

	glm::vec3 position = transformComponent->getLocalPosition();

	auto collisionComponent = entity->findComponent<CollisionComponent>();
	if (collisionComponent)
	{
		CollisionResult collisionResult = collisionComponent->resolveCollision(position, velocity, dt);

		position = collisionResult.adjustedPosition;
		velocity = collisionResult.adjustedVelocity;

		if (collisionResult.collidedAxis != glm::bvec3(0.0f))
		{
			velocity *= friction;
		}
	}

	transformComponent->setLocalPosition(position);
	transformComponent->offset(velocity * dt);
}

void PhysicsComponent::addForce(const glm::vec3& force)
{
	velocity += force;
}

std::unique_ptr<Component> PhysicsComponent::clone() const
{
	return std::make_unique<PhysicsComponent>(*this);
}
