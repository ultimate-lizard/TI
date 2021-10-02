#pragma once

#include <memory>
#include <optional>

#include <glm/glm.hpp>

#include <TI/Server/Component/Component.h>

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

	CollisionResult resolveCollision(const glm::vec3& position, const glm::vec3& velocity, const CollisionBox& testedCollisionBox, float dt);
	bool checkCollision(const glm::vec3& box1pos, const glm::vec3& box2pos, const CollisionBox& box1, const CollisionBox& box2);
	glm::vec3 calculateAabbDistanceTo(const glm::vec3& box1pos, const glm::vec3& box2pos, const CollisionBox& box1, const CollisionBox& box2);
	// Returns near and far points of the direction vector where collision happened. Returns none if collision fails
	std::optional<RayCollisionResult> checkRayVsAabb(const glm::vec3& origin, const glm::vec3& direction, const CollisionBox& box1, const glm::vec3& box2pos, const CollisionBox& box2);

	void setVelocity(const glm::vec3& velocity);
	const glm::vec3& getVelocity() const;
	void addVelocity(const glm::vec3& velocity);

	void setGravityEnabled(bool gravityEnabled);
	bool isGravityEnabled() const;

	void setFrictionEnabled(bool frictionEnabled);
	bool isFrictionEnabled() const;

	bool isOnGround() const;

	void setRenderCollisionBoxEnabled(bool renderBoxEnabled);
	bool isRenderCollisiobBoxEnabled() const;

	void setRenderCollisions(bool renderCollisions);
	bool isRenderCollisionsEnabled() const;

private:
	TransformComponent* transformComponent;
	CollisionBox collisionBox;

	float maxGravityVelocity;
	float currentGravityVelocity;

	glm::vec3 velocity;

	bool gravityEnabled;
	bool onGround;
	bool frictionEnabled;

	glm::vec3 previousPosition;

	bool renderCollisionBox;
	bool renderCollisions;
};
