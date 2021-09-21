#pragma once

#include <glm/glm.hpp>

#include <TI/Server/Component/Component.h>
#include <TI/Server/SceneNode.h>

class TransformComponent;
class PhysicsComponent;

class MovementComponent : public Component
{
public:
	MovementComponent();
	MovementComponent(const MovementComponent& otherMovementComp);
	MovementComponent(MovementComponent&&) = delete;

	void init() override;
	void tick(float dt) override;

	std::unique_ptr<Component> clone() const override;

	void setVelocity(const glm::vec3& velocity);
	const glm::vec3& getVelocity() const;

	void setOnGroundSpeed(float speed);
	float getOnGroundSpeed() const;

	float getSensivity() const;

	void setPitchRate(float pitchRate);
	float getPitchRate() const;

	void setYawRate(float yawRate);
	float getYawRate() const;

	void setMovementForward(float value);
	void setMovementSideways(float value);

	void jump();

	void setFlyModeEnabled(bool flyMode);
	bool isFlyModeEnabled() const;

	const glm::vec3& getHeadForward() const;
	const glm::vec3& getHeadRotation() const;

private:
	TransformComponent* transformComponent;
	PhysicsComponent* physicsComponent;

	glm::vec3 headRotation;
	glm::vec3 headForward;
	glm::vec3 movementDirection;

	float yawRate;
	float pitchRate;

	float onGroundSpeed;
	float inAirSpeed;
	glm::vec3 jumpVelocity;

	float lookSensivity;
	bool flightMode;

	bool wasOnGround;

public:
	void addHorizontalLook(float value);
	void addVerticalLook(float value);
};
