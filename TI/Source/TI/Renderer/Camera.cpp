#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(const glm::vec3& position) :
	position(position),
	forward(0.0f, 0.0f, -1.0f),
	up(0.0f, 1.0f, 0.0f),
	right(1.0f, 0.0f, 0.0f)
{

}

void Camera::setPerspective(float fov, float aspect, float near, float far)
{
	projection = glm::perspective(fov, aspect, near, far);
}

void Camera::updateView()
{
	view = glm::lookAt(position, position + forward, up);
}

const glm::mat4& Camera::getProjection() const
{
	return projection;
}

const glm::mat4& Camera::getView() const
{
	return view;
}

void Camera::setPosition(const glm::vec3& position)
{
	this->position = position;
}

const glm::vec3& Camera::getPosition() const
{
	return position;
}

void Camera::setForward(const glm::vec3& forward)
{
	this->forward = forward;
}

const glm::vec3& Camera::getForward() const
{
	return forward;
}

void Camera::setUp(const glm::vec3& up)
{
	this->up = up;
}

const glm::vec3& Camera::getUp() const
{
	return up;
}

void Camera::setRight(const glm::vec3& right)
{
	this->right = right;
}

const glm::vec3& Camera::getRight() const
{
	return right;
}
