#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <iostream>

Camera::Camera(const glm::vec3& position)
{
	this->position = position;
}

Camera::Camera(const Camera& otherCamera) :
	SceneNode(otherCamera)
{
	projection = otherCamera.projection;
	view = otherCamera.view;
}

void Camera::setPerspective(float fov, float aspect, float near, float far)
{
	projection = glm::perspective(fov, aspect, near, far);
}

void Camera::updateView()
{
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;
	glm::vec3 scew;
	glm::vec4 perspective;

	glm::decompose(transform, scale, rotation, position, scew, perspective);
	rotation = glm::conjugate(rotation);

	glm::vec3 forward = rotation * glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 up = rotation * glm::vec3(0.0f, 1.0f, 0.0f);

	view = glm::lookAt(position, position + forward, up);
}

const glm::mat4& Camera::getProjection() const
{
	return projection;
}

const glm::mat4& Camera::getView()
{
	updateView();

	return view;
}
