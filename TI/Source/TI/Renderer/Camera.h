#pragma once

#include <bitset>

#include <glm/glm.hpp>

class Camera
{
public:
	Camera(const glm::vec3& position = { 0.0f, 0.0f, 0.0f });

	void setPerspective(float fov, float aspect, float near, float far);
	void updateView();

	const glm::mat4& getProjection() const;
	const glm::mat4& getView() const;

	void setPosition(const glm::vec3& position);
	const glm::vec3& getPosition() const;

	void setVelocity(const glm::vec3& velocity);
	const glm::vec3& getVelocity() const;

	void update(float dt);

	void updateLookVectors();

	// MOVE THIS TO PLAYER ENTITY
	float getSpeed() const;

	const glm::vec3& getForward() const;

	void setPitch(float pitch);
	float getPitch() const;

	void setYaw(float yaw);
	float getYaw() const;

	float getSensivity() const;

	float yawRate;
	float pitchRate;

private:
	glm::mat4 projection;
	glm::mat4 view;

	glm::vec3 position;

	glm::vec3 forward;
	glm::vec3 up;
	glm::vec3 right;

	// MOVE THESE TO PLAYER ENTITY
	glm::vec3 velocity;
	float speed;

	float pitch;
	float yaw;
	float sensivity;


};
