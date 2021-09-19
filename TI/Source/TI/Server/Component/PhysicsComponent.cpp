#include "PhysicsComponent.h"

#include <TI/Server/Component/TransformComponent.h>
#include <TI/Server/Entity.h>
#include <TI/Server/Plane.h>
#include <TI/Client/DebugInformation.h>

PhysicsComponent::PhysicsComponent() :
	transformComponent(nullptr),
	currentGravityVelocity(0.0f),
	maxGravityVelocity(300.0f),
	velocity(0.0f),
	gravityEnabled(true),
	onGround(false),
	frictionEnabled(true)
{
	
}

PhysicsComponent::PhysicsComponent(const PhysicsComponent& other) :
	Component(other),
	transformComponent(other.transformComponent),
	currentGravityVelocity(other.currentGravityVelocity),
	collisionBox(other.collisionBox),
	maxGravityVelocity(other.maxGravityVelocity),
	velocity(other.velocity),
	gravityEnabled(other.gravityEnabled),
	onGround(other.onGround),
	frictionEnabled(other.frictionEnabled)
{

}

void PhysicsComponent::init()
{
	if (entity)
	{
		transformComponent = entity->findComponent<TransformComponent>();
	}
}

void PhysicsComponent::setVelocity(const glm::vec3& velocity)
{
	this->velocity = velocity;
}

const glm::vec3& PhysicsComponent::getVelocity() const
{
	return velocity;
}

void PhysicsComponent::addVelocity(const glm::vec3& velocity)
{
	this->velocity += velocity;
}

void PhysicsComponent::setGravityEnabled(bool gravityEnabled)
{
	this->gravityEnabled = gravityEnabled;
}

bool PhysicsComponent::isGravityEnabled() const
{
	return gravityEnabled;
}

void PhysicsComponent::setFrictionEnabled(bool frictionEnabled)
{
}

bool PhysicsComponent::isFrictionEnabled() const
{
	return false;
}

bool PhysicsComponent::isOnGround() const
{
	return onGround;
}

void PhysicsComponent::tick(float dt)
{
	if (transformComponent)
	{
		if (currentGravityVelocity >= maxGravityVelocity)
		{
			currentGravityVelocity = maxGravityVelocity;
		}

		if (gravityEnabled)
		{
			glm::vec3 gravityVector{ 0.0f, -1.0f, 0.0f };
			float gravityConstant = 3000.0f;
			glm::vec3 forces = gravityVector * gravityConstant;
			float mass = 1.0f;
			glm::vec3 acceleration = forces * mass;
			velocity += acceleration * dt;
		}

		if (frictionEnabled)
		{
			if (isOnGround() && (velocity.x || velocity.z || velocity.y))
			{
				velocity += -velocity * 120.0f * dt;
			}
		}

		glm::vec3 position = transformComponent->getPosition();
		CollisionResult result = applyCollision(position, velocity, collisionBox, dt);

		if (result.collidedAxis.x) velocity.x = result.adjustedVelocity.x;
		if (result.collidedAxis.y) velocity.y = result.adjustedVelocity.y;
		if (result.collidedAxis.z) velocity.z = result.adjustedVelocity.z;

		position += velocity * dt;

		onGround = velocity.y ? false : true;
		if (!isGravityEnabled()) onGround = false;

		// Note: If there ever be velocity glitches during collision, implement adjusted position here

		transformComponent->setPosition(position);
	}
}

std::unique_ptr<Component> PhysicsComponent::clone() const
{
	return std::make_unique<PhysicsComponent>(*this);
}

void PhysicsComponent::setCollisionBox(CollisionBox collisionBox)
{
	this->collisionBox = collisionBox;
}

CollisionResult PhysicsComponent::applyCollision(const glm::vec3& position, const glm::vec3& velocity, const CollisionBox& testedCollisionBox, float dt)
{
	CollisionResult result;

	for (int x = -1 + testedCollisionBox.min.x; x <= 1 + testedCollisionBox.max.x; ++x)
	{
		for (int y = -1 + testedCollisionBox.min.y; y <= 1 + testedCollisionBox.max.y; ++y)
		{
			for (int z = -1 + testedCollisionBox.min.z; z <= 1 + testedCollisionBox.max.z; ++z)
			{
				glm::vec3 currentPosition(position);
				currentPosition.x += x;
				currentPosition.y += y;
				currentPosition.z += z;
				
				if (Plane* plane = transformComponent->getPlane())
				{
					if (plane->getBlock(currentPosition) != 0)
					{
						glm::vec3 checkedVelocity(0.0f);
						checkedVelocity.x = velocity.x;
						if (checkCollision(position + checkedVelocity * dt, currentPosition, testedCollisionBox))
						{
							result.adjustedVelocity.x = 0;
							result.collidedAxis.x = true;
						}

						checkedVelocity = glm::vec3(0.0f);
						checkedVelocity.y = velocity.y;
						if (checkCollision(position + checkedVelocity * dt, currentPosition, testedCollisionBox))
						{
							result.adjustedVelocity.y = 0;
							result.collidedAxis.y = true;
						}

						checkedVelocity = glm::vec3(0.0f);
						checkedVelocity.z = velocity.z;
						if (checkCollision(position + checkedVelocity * dt, currentPosition, testedCollisionBox))
						{
							result.adjustedVelocity.z = 0;
							result.collidedAxis.z = true;
						}
					}
				}
			}
		}
	}

	return result;
}

bool PhysicsComponent::checkCollision(const glm::vec3& boxPosition, const glm::vec3& planePosition, const CollisionBox& testedCollisionBox)
{
	bool result = false;

	CollisionBox box;
	box.min = boxPosition;
	box.min.x += testedCollisionBox.min.x;
	box.min.y += testedCollisionBox.min.y;
	box.min.z += testedCollisionBox.min.z;

	box.max = boxPosition;
	box.max.x += testedCollisionBox.max.x;
	box.max.y += testedCollisionBox.max.y;
	box.max.z += testedCollisionBox.max.z;

	glm::ivec3 currentPlanePosition = planePosition;
	if ((box.min.x <= currentPlanePosition.x + 1 && box.max.x >= currentPlanePosition.x) &&
		(box.min.y <= currentPlanePosition.y + 1 && box.max.y >= currentPlanePosition.y) &&
		(box.min.z <= currentPlanePosition.z + 1 && box.max.z >= currentPlanePosition.z))
	{
		result = true;
	}

	return result;
}
