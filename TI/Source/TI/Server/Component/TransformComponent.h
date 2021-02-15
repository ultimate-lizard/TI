#pragma once

#include <glm/glm.hpp>

#include <TI/Server/Component/Component.h>

class TransformComponent : public Component
{
public:
	TransformComponent(const glm::vec3& position = { 0.0f, 0.0f, 0.0f }, const glm::vec3& rotation = { 0.0f, 0.0f, 0.0f });

	void setPosition(const glm::vec3& position);
	const glm::vec3& getPosition();

	void setPitch(float pitch);
	float getPitch() const;

	void setYaw(float yaw);
	float getYaw() const;

	void setRoll(float roll);
	float getRoll() const;

	const glm::mat4 getTransform() const;

	void setScale(const glm::vec3& scale);
	const glm::vec3& getScale() const;

private:
	glm::vec3 position;
	glm::vec3 scale;

	float pitch;
	float yaw;
	float roll;
};
