#pragma once

#include <glm/glm.hpp>

#include <TI/Server/Component/Component.h>

class TransformComponent : public Component
{
public:
	TransformComponent(Entity* entity, const glm::vec3& position = { 0.0f, 0.0f, 0.0f }, const glm::vec3& rotation = { 0.0f, 0.0f, 0.0f });

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
