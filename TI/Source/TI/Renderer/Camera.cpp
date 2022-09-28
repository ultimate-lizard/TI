#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <iostream>

Camera::Camera(const glm::vec3& position)
{
	setLocalPosition(position);
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
	glm::vec3 absolutePosition = getDerivedPosition(cs);
	glm::quat absoluteRotation = getDerivedOrientation(cs);

	// glm::decompose(getTransform(cs), scale, absoluteRotation, absolutePosition, scew, perspective);
	
	// absoluteRotation = glm::conjugate(absoluteRotation);

	//std::cout << absoluteRotation.x << " " << absoluteRotation.y << " " << absoluteRotation.z << std::endl;

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
