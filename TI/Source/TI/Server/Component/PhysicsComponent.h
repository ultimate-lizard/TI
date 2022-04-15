#pragma once

#include <memory>
#include <optional>

#include <glm/glm.hpp>

#include <TI/Server/Component/Component.h>
#include <TI/Server/SceneNode.h>

class TransformComponent;

struct CollisionBox
{
	glm::vec3 size;
	glm::vec3 offset;
};

struct CollisionResult
{
	glm::bvec3 collidedAxis;
	glm::vec3 adjustedVelocity;
	glm::vec3 adjustedPosition;
};

struct RayCollisionResult
{
	float near;
	float far;
	glm::vec3 normal;
};

class PhysicsComponent : public Component
{
	enum class Axis
	{
		X,
		Y,
		Z
	};

public:
	PhysicsComponent();
	PhysicsComponent(const PhysicsComponent& other);
	PhysicsComponent(PhysicsComponent&&) = delete;

	void init() override;
	void tick(float dt) override;

	std::unique_ptr<Component> clone() const override;

	void setCollisionBox(CollisionBox collisionBox);
	CollisionBox getCollisionBox() const;

	CollisionResult resolveCollision(const glm::vec3& position, const glm::vec3& velocity, float dt);
	
	void setRenderCollisionBoxEnabled(bool renderBoxEnabled);
	bool isRenderCollisiobBoxEnabled() const;

	void setRenderCollisions(bool renderCollisions);
	bool isRenderCollisionsEnabled() const;

	std::optional<RayCollisionResult> checkRayVsAabb(const glm::vec3& origin, const glm::vec3& direction, const glm::vec3& box2pos, const CollisionBox& box2);
	bool checkCollision(const glm::vec3& box1pos, const glm::vec3& box2pos, const CollisionBox& box1, const CollisionBox& box2);

private:
	glm::vec3 calculateAabbDistanceTo(const glm::vec3& box1pos, const glm::vec3& box2pos, const CollisionBox& box1, const CollisionBox& box2);

private:
	TransformComponent* transformComponent;
	CollisionBox collisionBox;

	bool renderCollisionBox;
	bool renderCollisions;
};
