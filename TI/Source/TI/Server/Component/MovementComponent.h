#pragma once

#include <glm/glm.hpp>

#include <TI/Server/Component/Component.h>

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

	void setVelocity(const glm::vec3& velocity);
	const glm::vec3& getVelocity() const;

	void setSpeed(float speed);
	float getSpeed() const;

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

	std::unique_ptr<Component> clone() const override;

	const glm::vec3& getForward() const;

private:
	TransformComponent* transformComponent;
	PhysicsComponent* physicsComponent;

	float yawRate;
	float pitchRate;

	glm::vec3 movementDirection;
	glm::vec3 velocity;
	float speed;

	glm::vec3 forward;

	float sensivity;
	bool flyMode;

public:
	void addHorizontalLook(float value);
	void addVerticalLook(float value);
};