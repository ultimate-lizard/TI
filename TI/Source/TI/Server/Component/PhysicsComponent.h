#pragma once

#include <memory>
#include <optional>

#include <glm/glm.hpp>

#include <TI/Server/Component/Component.h>

class TransformComponent;

struct CollisionBox
{
	glm::vec3 min = glm::vec3(0.0f);
	glm::vec3 max = glm::vec3(0.0f);
};

struct CollisionResult
{
	glm::bvec3 collidedAxis;
	glm::vec3 adjustedPosition;
	glm::vec3 adjustedVelocity;
};

class PhysicsComponent : public Component
{
public:
	PhysicsComponent();
	PhysicsComponent(const PhysicsComponent& other);
	PhysicsComponent(PhysicsComponent&&) = delete;

	void init() override;
	void tick(float dt) override;

	std::unique_ptr<Component> clone() const override;

	void setCollisionBox(CollisionBox collisionBox);

	CollisionResult applyCollision(const glm::vec3& position, const glm::vec3& velocity, const CollisionBox& testedCollisionBox, float dt);
	bool checkCollision(const glm::vec3& boxPosition, const glm::vec3& planePosition, const CollisionBox& testedCollisionBox);

	void setVelocity(const glm::vec3& velocity);
	const glm::vec3& getVelocity() const;
	void addVelocity(const glm::vec3& velocity);

	void setGravityEnabled(bool gravityEnabled);
	bool isGravityEnabled() const;

	void setFrictionEnabled(bool frictionEnabled);
	bool isFrictionEnabled() const;

	bool isOnGround() const;

private:
	TransformComponent* transformComponent;
	CollisionBox collisionBox;

	float maxGravityVelocity;
	float currentGravityVelocity;

	glm::vec3 velocity;

	bool gravityEnabled;
	bool onGround;
	bool frictionEnabled;
};
