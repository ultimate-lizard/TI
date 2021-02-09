#include "Component.h"

#include <TI/Server/Entity.h>
#include <TI/Renderer/Camera.h>

MovementComponent::MovementComponent(Entity* entity) :
	Component(entity),
	yawRate(0.0f),
	pitchRate(0.0f),
	speed(10.0f),
	pitch(0.0f),
	yaw(-90.0f),
	forward(0.0f, 0.0f, -1.0f),
	up({0.0f, 1.0f, 0.0f}),
	right({1.0f, 0.0f, 0.0f}),
	sensivity(20.0f)
{

}

void MovementComponent::tick(float dt)
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

	glm::vec3 newForward;
	newForward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	newForward.y = sin(glm::radians(pitch));
	newForward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	forward = glm::normalize(newForward);

	right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
	up = glm::normalize(glm::cross(right, forward));

	if (entity)
	{
		auto transformComp = entity->findComponent<TransformComponent>();
		if (transformComp)
		{
			glm::vec3 position = transformComp->getPosition();
			position += getVelocity().z * (speed * forward) * dt;
			position += getVelocity().x * (speed * glm::cross(forward, up)) * dt;
			transformComp->setPosition(position);

			CameraComponent* cameraComp = entity->findComponent<CameraComponent>();
			if (cameraComp)
			{
				Camera* camera = cameraComp->getCamera();
				if (camera)
				{
					camera->setPosition(position);
					camera->setForward(forward);
					camera->setRight(right);
					camera->setUp(up);
				}
			}
		}
	}
}

float MovementComponent::getSensivity() const
{
	return sensivity;
}

void MovementComponent::setPitchRate(float pitchRate)
{
	this->pitchRate = pitchRate;
}

float MovementComponent::getPitchRate() const
{
	return pitchRate;
}

void MovementComponent::setYawRate(float yawRate)
{
	this->yawRate = yawRate;
}

float MovementComponent::getYawRate() const
{
	return yawRate;
}

CameraComponent::CameraComponent(Entity* entity) :
	Component(entity),
	camera(nullptr)
{

}

void CameraComponent::setCamera(Camera* const camera)
{
	this->camera = camera;
}

Camera* const CameraComponent::getCamera()
{
	return camera;
}
