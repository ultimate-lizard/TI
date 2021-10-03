#pragma once

#include <glm/glm.hpp>

#include <TI/Server/Component/Component.h>
#include <TI/Server/SceneNode.h>

class TransformComponent;
class PhysicsComponent;

class MovementComponent : public Component
{
	enum class MovementState
	{
		Walk,
		Fall,
		Fly
	};

public:
	MovementComponent();
	MovementComponent(const MovementComponent&) = default;
	MovementComponent(MovementComponent&&) = delete;

	void init() override;
	void tick(float dt) override;

	std::unique_ptr<Component> clone() const override;

	void jump();

	float getLookSensivity() const;
	void setLookSensivity(float sensivity);

	void setPitchRate(float pitchRate);
	float getPitchRate() const;

	void setYawRate(float yawRate);
	float getYawRate() const;

	void addHorizontalLook(float value);
	void addVerticalLook(float value);

	void setMovementForward(float value);
	void setMovementSideways(float value);

	void setWalkAcceleration(float acceleration);
	float getWalkAcceleration() const;

	void setWalkAccelerationInAir(float acceleration);
	float getWalkAccelerationInAir() const;

	void setWalkMaxSpeed(float maxSpeed);
	float getWalkMaxSpeed() const;

	void setFlightSpeed(float flightSpeed);
	float getFlightSpeed() const;

	void setBreakingFactor(float breakingFactor);
	float getBreakingFactor() const;

	void setFlyModeEnabled(bool flyMode);
	bool isFlightModeEnabled() const;

	const glm::vec3& getHeadDirection() const;
	const glm::vec3& getHeadRotation() const;

private:
	void handleInput(float dt);

	void handleWalk(float dt);
	void handleFall(float dt);
	void handleFlight(float dt);

private:
	TransformComponent* transformComponent;
	PhysicsComponent* physicsComponent;

	glm::vec3 headRotation;
	glm::vec3 headDirection;
	glm::vec3 walkDirection;
	glm::vec3 input;

	float yawRate;
	float pitchRate;

	glm::vec3 velocity;

	float walkAcceleration;
	float walkAccelerationInAir;
	float walkMaxSpeed;
	float flightSpeed;
	float breakingFactor;

	glm::vec3 jumpVelocity;

	float lookSensivity;

	bool flightMode;

	MovementState movementState;
};
