#pragma once

#include <glm/glm.hpp>

#include <TI/Server/Component/Component.h>
#include <TI/Server/SceneNode.h>
#include <TI/Server/PlaneSide.h>

class TransformComponent;
class PhysicsComponent;
class CelestialBody;

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
	glm::vec3 getHeadPosition() const;

	void setVelocity(const glm::vec3& newVelocity);

private:
	void handleInput(float dt);

	void handleWalk(float dt);
	void handleFall(float dt);
	void handleFlight(float dt);

	glm::vec3 getGravityVector() const;

	void updatePlaneSideRotation(float dt);

private:
	TransformComponent* transformComponent;
	PhysicsComponent* physicsComponent;

	glm::vec3 headPosition;
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
	float crawlMaxSpeed;
	float flightSpeed;
	float breakingFactor;

	float jumpVelocity;

	bool flightMode;

	MovementState movementState;

	// Planet side rotation members
	OrientationInfo orientationInfo;
	OrientationInfo previousOrientationInfo;

	bool shouldRotate;
	bool planeSideTransitionInProgress;
	float currentRotationAngle;
	glm::vec3 sideRotationAxis;

	float rotationStartHeight = 0.0f;
	float pendingRotationAngle;

	CelestialBody* primaryBody;
};
