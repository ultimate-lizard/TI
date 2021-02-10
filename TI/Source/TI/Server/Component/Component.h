#pragma once

#include <memory>

#include <glm/glm.hpp>

class Entity;
class Camera;

class Component
{
public:
	Component(Entity* entity) : entity(entity) {}
	virtual ~Component() = default;

	virtual void tick(float dt) {}

protected:
	Entity* entity;
};

class TransformComponent : public Component
{
public:
	TransformComponent(Entity* entity, const glm::vec3& position = { 0.0f, 0.0f, 0.0f }, const glm::vec3& rotation = {0.0f, 0.0f, 0.0f});

	void setPosition(const glm::vec3& position);
	const glm::vec3& getPosition();

	void setPitch(float pitch);
	float getPitch() const;

	void setYaw(float yaw);
	float getYaw() const;

	void setRoll(float roll);
	float getRoll() const;

private:
	glm::vec3 position;

	float pitch;
	float yaw;
	float roll;
};

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

class CameraComponent : public Component
{
public:
	CameraComponent(Entity* entity);

	void setCamera(std::unique_ptr<Camera> camera);
	Camera* getCamera() const;

private:
	std::unique_ptr<Camera> camera;
};
