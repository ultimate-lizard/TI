#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(const glm::vec3& position) :
	position(position),
	forward(0.0f, 0.0f, -1.0f),
	up(0.0f, 1.0f, 0.0f),
	right(1.0f, 0.0f, 0.0f),
	speed(10.0f),
	pitch(0.0f),
	yaw(-90.0f),
	sensivity(20.0f),
	pitchRate(0.0f),
	yawRate(0.0f)
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

void Camera::setVelocity(const glm::vec3& velocity)
{
	this->velocity = velocity;
}

const glm::vec3& Camera::getVelocity() const
{
	return velocity;
}

void Camera::update(float dt)
{
	pitch += pitchRate * dt;
	yaw += yawRate * dt;

	if (pitch > 89.0f)
	{
		setPitch(89.0f);
	}
	if (pitch < -89.0f)
	{
		setPitch(-89.0f);
	}

	updateLookVectors();

	position += getVelocity().z * (speed * forward) * dt;
	position += getVelocity().x * (speed * glm::cross(forward, up)) * dt;
}

void Camera::updateLookVectors()
{
	glm::vec3 newForward;
	newForward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	newForward.y = sin(glm::radians(pitch));
	newForward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	forward = glm::normalize(newForward);

	right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
	up = glm::normalize(glm::cross(right, forward));
}

float Camera::getSpeed() const
{
	return speed;
}

const glm::vec3& Camera::getForward() const
{
	return forward;
}

void Camera::setPitch(float pitch)
{
	this->pitch = pitch;
}

float Camera::getPitch() const
{
	return pitch;
}

void Camera::setYaw(float yaw)
{
	this->yaw = yaw;
}

float Camera::getYaw() const
{
	return yaw;
}

float Camera::getSensivity() const
{
	return sensivity;
}
