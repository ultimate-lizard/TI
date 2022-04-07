#pragma once

#include <bitset>

#include <glm/glm.hpp>

#include <TI/Server/SceneNode.h>

class Camera : public SceneNode
{
public:
	Camera(const glm::vec3& position = { 0.0f, 0.0f, 0.0f });
	Camera(const Camera& other);

	void setPerspective(float fov, float aspect, float near, float far);
	void updateView();

	const glm::mat4& getProjection() const;
	const glm::mat4& getView();

	void setForward(const glm::vec3& forward);

private:
	glm::mat4 projection;
	glm::mat4 view;
	glm::vec3 forward;
};
