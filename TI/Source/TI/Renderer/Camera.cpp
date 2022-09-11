#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <iostream>

Camera::Camera(const glm::vec3& position)
{
	setPosition(position);
}

Camera::Camera(const Camera& other) :
	SceneMultiNode(other),
	projection(other.projection),
	view(other.view)
{
}

void Camera::setPerspective(float fov, float aspect, float near, float far)
{
	projection = glm::perspective(fov, aspect, near, far);
}

void Camera::updateView(CoordinateSystem cs)
{
	glm::vec3 absolutePosition;
	glm::quat absoluteRotation;
	glm::vec3 scale;
	glm::vec3 scew;
	glm::vec4 perspective;

	glm::decompose(getTransform(cs), scale, absoluteRotation, absolutePosition, scew, perspective);
	
	absoluteRotation = glm::conjugate(absoluteRotation);

	glm::vec3 forward = absoluteRotation * glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 up = absoluteRotation * glm::vec3(0.0f, 1.0f, 0.0f);

	view = glm::lookAt(absolutePosition, absolutePosition + forward, up);
}

const glm::mat4& Camera::getProjection() const
{
	return projection;
}

const glm::mat4& Camera::getView(CoordinateSystem cs)
{
	updateView(cs);

	return view;
}

void Camera::setForward(const glm::vec3& forward)
{
	this->forward = forward;
}
