#pragma once

#include <glm/glm.hpp>

#include <TI/Server/Component/Component.h>

class MovementComponent : public Component
{
public:
	MovementComponent(Entity* entity);

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

	void setPitch(float pitch);
	float getPitch() const;

	void setYaw(float yaw);
	float getYaw() const;

	void setMovementForward(float value);
	void setMovementSideways(float value);

private:
	float yawRate;
	float pitchRate;

	glm::vec3 velocity;
	float speed;

	glm::vec3 forward;
	glm::vec3 up;
	glm::vec3 right;

	float sensivity;
public:
	void addHorizontalLook(float value);
	void addVerticalLook(float value);
};