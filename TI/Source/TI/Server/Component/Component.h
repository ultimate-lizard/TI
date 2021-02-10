#pragma once

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
	TransformComponent(Entity* entity, const glm::vec3& position = {0.0f, 0.0f, 0.0f}) : Component(entity) {}

	const glm::vec3& getPosition()
	{
		return position;
	}

	void setPosition(const glm::vec3& position)
	{
		this->position = position;
	}

private:
	glm::vec3 position;
};

class MovementComponent : public Component
{
public:
	MovementComponent(Entity* entity);

	void tick(float dt) override;

	void setVelocity(const glm::vec3& velocity)
	{
		this->velocity = velocity;
	}

	const glm::vec3& getVelocity() const
	{
		return velocity;
	}

	void setPitch(float pitch)
	{
		this->pitch = pitch;
	}

	float getPitch() const
	{
		return pitch;
	}

	void setYaw(float yaw)
	{
		this->yaw = yaw;
	}

	float getYaw() const
	{
		return yaw;
	}

	float getSpeed() const
	{
		return speed;
	}

	float getSensivity() const;

	void setPitchRate(float pitchRate);
	float getPitchRate() const;
	
	void setYawRate(float yawRate);
	float getYawRate() const;

private:
	float yawRate;
	float pitchRate;

	glm::vec3 velocity;
	float speed;

	float pitch;
	float yaw;

	glm::vec3 forward;
	glm::vec3 up;
	glm::vec3 right;

	float sensivity;
};

class CameraComponent : public Component
{
public:
	CameraComponent(Entity* entity);

	void setCamera(Camera* const camera);
	Camera* const getCamera();

private:
	Camera* camera;
};
