#pragma once

#include <bitset>

#include <glm/glm.hpp>

class Camera
{
public:
	Camera(const glm::vec3& position = { 0.0f, 0.0f, 0.0f });
	Camera(const Camera& otherCamera);

	void setPerspective(float fov, float aspect, float near, float far);
	void updateView();

	const glm::mat4& getProjection() const;
	const glm::mat4& getView();

	void setPosition(const glm::vec3& position);
	const glm::vec3& getPosition() const;

	void setForward(const glm::vec3& forward);
	const glm::vec3& getForward() const;

	void setUp(const glm::vec3& up);
	const glm::vec3& getUp() const;

	void setRight(const glm::vec3& right);
	const glm::vec3& getRight() const;

	void operator=(const Camera& otherCamera);

private:
	glm::mat4 projection;
	glm::mat4 view;

	glm::vec3 position;

	glm::vec3 forward;
	glm::vec3 up;
	glm::vec3 right;
};
