#include "PhysicsComponent.h"

#include <algorithm>
#include <iomanip>
#include <limits>

#include <TI/Server/Component/TransformComponent.h>
#include <TI/Server/Component/MovementComponent.h>
#include <TI/Server/Entity.h>
#include <TI/Server/Plane.h>
#include <TI/Client/DebugInformation.h>

PhysicsComponent::PhysicsComponent() :
	Component(),
	transformComponent(nullptr),
	renderCollisionBox(false),
	renderCollisions(false)
{
	
}

PhysicsComponent::PhysicsComponent(const PhysicsComponent& other) :
	Component(other),
	transformComponent(other.transformComponent),
	collisionBox(other.collisionBox),
	renderCollisionBox(other.renderCollisionBox),
	renderCollisions(other.renderCollisions)
{

}

void PhysicsComponent::setRenderCollisionBoxEnabled(bool renderBoxEnabled)
{
	this->renderCollisionBox = renderBoxEnabled;
}

bool PhysicsComponent::isRenderCollisiobBoxEnabled() const
{
	return renderCollisionBox;
}

void PhysicsComponent::setRenderCollisions(bool renderCollisions)
{
	this->renderCollisions = renderCollisions;
}

bool PhysicsComponent::isRenderCollisionsEnabled() const
{
	return renderCollisions;
}

void PhysicsComponent::init()
{
	if (entity)
	{
		transformComponent = entity->findComponent<TransformComponent>();
	}
}

void PhysicsComponent::tick(float dt)
{
	if (renderCollisionBox)
	{
		if (transformComponent)
		{
			glm::vec3 position = transformComponent->getPosition();
			drawDebugBox({ position.x + collisionBox.getOffset().x - collisionBox.getSize().x / 2.0f, position.y + collisionBox.getOffset().y - collisionBox.getSize().y / 2.0f , position.z + collisionBox.getOffset().z - collisionBox.getSize().z / 2.0f}, collisionBox.getSize(), {0.0f, 1.0f, 0.0f, 1.0f}, 1.0f, false);
		}
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

CollisionBox PhysicsComponent::getCollisionBox() const
{
	return collisionBox;
}

CollisionResult PhysicsComponent::resolveCollision(const glm::vec3& position, const glm::vec3& velocity, float dt)
{
	CollisionResult result;
	result.adjustedPosition = position + collisionBox.getOffset();
	result.adjustedVelocity = velocity;

	std::vector<std::pair<float, glm::vec3>> collisions;

	// With more speed take more blocks into account to prevent tunneling
	float xRangeMin = velocity.x < 0.0f ? velocity.x * dt : 0.0f;
	float xRangeMax = velocity.x > 0.0f ? velocity.x * dt : 0.0f;
	float yRangeMin = velocity.y < 0.0f ? velocity.y * dt : 0.0f;
	float yRangeMax = velocity.y > 0.0f ? velocity.y * dt : 0.0f;
	float zRangeMin = velocity.z < 0.0f ? velocity.z * dt : 0.0f;
	float zRangeMax = velocity.z > 0.0f ? velocity.z * dt : 0.0f;

	for (int x = -1 - static_cast<int>(collisionBox.getSize().x / 2.0f + xRangeMin); x <= 1 + static_cast<int>(collisionBox.getSize().x / 2.0f + xRangeMax); ++x)
	{
		for (int y = -1 - static_cast<int>(collisionBox.getSize().y / 2.0f + yRangeMin); y <= 1 + static_cast<int>(collisionBox.getSize().y / 2.0f + yRangeMax); ++y)
		{
			for (int z = -1 - static_cast<int>(collisionBox.getSize().z / 2.0f + zRangeMin); z <= 1 + static_cast<int>(collisionBox.getSize().z / 2.0f + zRangeMax); ++z)
			{
				glm::vec3 blockPosition = glm::ivec3(result.adjustedPosition);
				blockPosition.x += x;
				blockPosition.y += y;
				blockPosition.z += z;

				glm::vec3 blockCenterPosition = blockPosition;
				blockCenterPosition.x += 0.5f;
				blockCenterPosition.y += 0.5f;
				blockCenterPosition.z += 0.5f;

				if (transformComponent->getPlane()->getBlock(blockPosition) != 0)
				{
					if (renderCollisions)
					{
						drawDebugBox(blockPosition, glm::vec3(1.0f), { 1.0f, 1.0f, 0.0f, 1.0f }, 1.0f, false);
					}

					float differenceX = std::numeric_limits<float>::max();
					float differenceY = std::numeric_limits<float>::max();
					float differenceZ = std::numeric_limits<float>::max();

					std::vector<std::pair<float, Axis>> differences;

					glm::vec3 distance = calculateAabbDistanceTo(result.adjustedPosition, blockCenterPosition, collisionBox, { glm::vec3(1.0), glm::vec3(0.0f) });

					if (result.adjustedVelocity.x)
					{
						differenceX = glm::abs(distance.x / result.adjustedVelocity.x);
					}
					if (result.adjustedVelocity.y)
					{
						differenceY = glm::abs(distance.y / result.adjustedVelocity.y);
					}
					if (result.adjustedVelocity.z)
					{
						differenceZ = glm::abs(distance.z / result.adjustedVelocity.z);
					}
					
					differences.push_back({ differenceX, Axis::X });
					differences.push_back({ differenceY, Axis::Y });
					differences.push_back({ differenceZ, Axis::Z });

					std::sort(differences.begin(), differences.end(), [](const std::pair<float, Axis>& a, const std::pair<float, Axis>& b) {
						return a.first < b.first;
					});

					for (const std::pair<float, Axis>& pair : differences)
					{
						switch (pair.second)
						{
						case Axis::X:
						{
							glm::vec3 newPosition{ result.adjustedPosition + glm::vec3(result.adjustedVelocity.x, 0.0f, 0.0f) * dt };
							if (checkCollision(newPosition, blockCenterPosition, collisionBox, { glm::vec3(1.0f), glm::vec3(0.0f) }))
							{
								if (velocity.x < 0.0f)
									result.adjustedPosition.x = blockPosition.x + 1.0f + collisionBox.getSize().x / 2.0f;
								else if (velocity.x > 0.0f)
									result.adjustedPosition.x = blockPosition.x - collisionBox.getSize().x / 2.0f;

								result.adjustedVelocity.x = 0.0f;
								result.collidedAxis.x = true;
							}

							break;
						}

						case Axis::Y:
						{
							glm::vec3 newPosition{ result.adjustedPosition + glm::vec3(0.0f, result.adjustedVelocity.y, 0.0f) * dt };
							if (checkCollision(newPosition, blockCenterPosition, collisionBox, { glm::vec3(1.0f), glm::vec3(0.0f) }))
							{
								if (velocity.y < 0.0f)
									result.adjustedPosition.y = blockPosition.y + 1.0f + collisionBox.getSize().y / 2.0f;
								else if (velocity.y > 0.0f)
									result.adjustedPosition.y = blockPosition.y - collisionBox.getSize().y / 2.0f;

								result.adjustedVelocity.y = 0.0f;
								result.collidedAxis.y = true;
							}

							break;
						}

						case Axis::Z:
						{
							glm::vec3 newPosition{ result.adjustedPosition + glm::vec3(0.0f, 0.0f, result.adjustedVelocity.z) * dt };
							if (checkCollision(newPosition, blockCenterPosition, collisionBox, { glm::vec3(1.0f), glm::vec3(0.0f) }))
							{
								if (velocity.z < 0.0f)
									result.adjustedPosition.z = blockPosition.z + 1.0f + collisionBox.getSize().z / 2.0f;
								else if (velocity.z > 0.0f)
									result.adjustedPosition.z = blockPosition.z - collisionBox.getSize().z / 2.0f;

								result.adjustedVelocity.z = 0.0f;
								result.collidedAxis.z = true;
							}

							break;
						}
						}
					}
				}
			}
		}
	}

	result.adjustedPosition -= collisionBox.getOffset();
	return result;
}

bool PhysicsComponent::checkCollision(const glm::vec3& box1pos, const glm::vec3& box2pos, const CollisionBox& box1, const CollisionBox& box2)
{
	if ((box1pos.x - box1.getSize().x / 2.0f < box2pos.x + box2.getSize().x / 2.0f && box1pos.x + box1.getSize().x / 2.0f > box2pos.x - box2.getSize().x / 2.0f) &&
		(box1pos.y - box1.getSize().y / 2.0f < box2pos.y + box2.getSize().y / 2.0f && box1pos.y + box1.getSize().y / 2.0f > box2pos.y - box2.getSize().y / 2.0f) &&
		(box1pos.z - box1.getSize().z / 2.0f < box2pos.z + box2.getSize().z / 2.0f && box1pos.z + box1.getSize().z / 2.0f > box2pos.z - box2.getSize().z / 2.0f))
	{
		if (renderCollisions)
		{
			drawDebugBox(glm::vec3(box2pos.x - 0.5f, box2pos.y - 0.5f, box2pos.z - 0.5f), glm::vec3(1.0f), { 1.0f, 0.0f, 0.0f, 1.0f }, 5.0f, false);
		}
		
		return true;
	}

	return false;
}

glm::vec3 PhysicsComponent::calculateAabbDistanceTo(const glm::vec3& box1pos, const glm::vec3& box2pos, const CollisionBox& box1, const CollisionBox& box2)
{
	glm::vec3 distance;

	if (box1pos.x < box2pos.x)
	{
		distance.x = (box2pos.x - box2.getSize().x / 2.0f) - (box1pos.x + box1.getSize().x / 2.0f);
	}
	else if (box1pos.x > box2pos.x)
	{
		distance.x = (box1pos.x - box1.getSize().x / 2.0f) - (box2pos.x + box2.getSize().x / 2.0f);
	}

	if (box1pos.y < box2pos.y)
	{
		distance.y = (box2pos.y - box2.getSize().y / 2.0f) - (box1pos.y + box1.getSize().y / 2.0f);
	}
	else if (box1pos.y > box2pos.y)
	{
		distance.y = (box1pos.y - box1.getSize().y / 2.0f) - (box2pos.y + box2.getSize().y / 2.0f);
	}

	if (box1pos.z < box2pos.z)
	{
		distance.z = (box2pos.z - box2.getSize().z / 2.0f) - (box1pos.z + box1.getSize().z / 2.0f);
	}
	else if (box1pos.z > box2pos.z)
	{
		distance.z = (box1pos.z - box1.getSize().z / 2.0f) - (box2pos.z + box2.getSize().z / 2.0f);
	}

	return distance;
}

std::optional<RayCollisionResult> PhysicsComponent::checkRayVsAabb(const glm::vec3& origin, const glm::vec3& direction, const glm::vec3& box2pos, const CollisionBox& box2)
{
	glm::vec3 near;
	near.x = (box2pos.x - origin.x) / direction.x;
	near.y = (box2pos.y - origin.y) / direction.y;
	near.z = (box2pos.z - origin.z) / direction.z;

	glm::vec3 far;
	far.x = (box2pos.x + box2.getSize().x - origin.x) / direction.x;
	far.y = (box2pos.y + box2.getSize().y - origin.y) / direction.y;
	far.z = (box2pos.z + box2.getSize().z - origin.z) / direction.z;

	if (std::isnan(near.x) || std::isnan(near.y) || std::isnan(near.z)) return {};
	if (std::isnan(far.x) || std::isnan(far.y) || std::isnan(far.z)) return {};

	if (near.x > far.x) std::swap(near.x, far.x);
	if (near.y > far.y) std::swap(near.y, far.y);
	if (near.z > far.z) std::swap(near.z, far.z);

	if (near.x > far.y || near.y > far.x || near.z > far.y || near.y > far.z || near.x > far.z || near.z > far.x) return {};

	float hitNear = std::max({ near.x, near.y, near.z });
	float hitFar = std::min({ far.x, far.y, far.z });

	if (hitFar < 0.0f) return {};

	glm::ivec3 normal;
	
	if (near.x > near.y && near.x > near.z)
	{
		if (direction.x < 0)
		{
			normal = glm::ivec3{ 1, 0, 0 };
		}
		else
		{
			normal = glm::ivec3{ -1, 0, 0 };
		}
	}
	else if (near.x < near.y && near.z < near.y)
	{
		if (direction.y < 0)
		{
			normal = glm::ivec3{ 0, 1, 0 };
		}
		else
		{
			normal = glm::ivec3{ 0, -1, 0 };
		}
	}
	else if (near.z > near.y && near.z > near.x)
	{
		if (direction.z < 0)
		{
			normal = glm::ivec3{ 0, 0, 1 };
		}
		else
		{
			normal = glm::ivec3{ 0, 0, -1 };
		}
	}

	return { { hitNear, hitFar, normal } };
}
