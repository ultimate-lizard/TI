#include "MovementComponent.h"

#include <TI/Server/Entity.h>
#include <TI/Server/Component/CameraComponent.h>
#include <TI/Server/Component/TransformComponent.h>
#include <TI/Renderer/Camera.h>

MovementComponent::MovementComponent(Entity* entity) :
	Component(entity),
	yawRate(0.0f),
	pitchRate(0.0f),
	speed(10.0f),
	forward(0.0f, 0.0f, -1.0f),
	up({ 0.0f, 1.0f, 0.0f }),
	right({ 1.0f, 0.0f, 0.0f }),
	sensivity(20.0f)
{

}

void MovementComponent::tick(float dt)
{
	if (entity)
	{
		auto transformComp = entity->findComponent<TransformComponent>();
		if (transformComp)
		{
			float pitch = transformComp->getPitch();
			float yaw = transformComp->getYaw();

			pitch += pitchRate * dt;
			yaw += yawRate * dt;

			if (pitch > 89.0f)
			{
				transformComp->setPitch(89.0f);
			}
			if (pitch < -89.0f)
			{
				transformComp->setPitch(-89.0f);
			}

			glm::vec3 newForward;
			newForward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
			newForward.y = sin(glm::radians(pitch));
			newForward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

			forward = glm::normalize(newForward);

			right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
			up = glm::normalize(glm::cross(right, forward));

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

void MovementComponent::setVelocity(const glm::vec3& velocity)
{
	this->velocity = velocity;
}

const glm::vec3& MovementComponent::getVelocity() const
{
	return velocity;
}

void MovementComponent::setSpeed(float speed)
{
	this->speed = speed;
}

float MovementComponent::getSpeed() const
{
	return speed;
}

float MovementComponent::getSensivity() const
{
	return sensivity;
}

void MovementComponent::setPitchRate(float pitchRate)
{
	this->pitchRate = pitchRate * sensivity;
}

float MovementComponent::getPitchRate() const
{
	return pitchRate;
}

void MovementComponent::setYawRate(float yawRate)
{
	this->yawRate = yawRate * sensivity;
}

float MovementComponent::getYawRate() const
{
	return yawRate;
}

void MovementComponent::setPitch(float pitch)
{
	if (entity)
	{
		auto transformComp = entity->findComponent<TransformComponent>();
		if (transformComp)
		{
			transformComp->setPitch(pitch);
		}
	}
}

float MovementComponent::getPitch() const
{
	if (entity)
	{
		auto transformComp = entity->findComponent<TransformComponent>();
		if (transformComp)
		{
			return transformComp->getPitch();
		}
	}

	return 0.0f;
}

void MovementComponent::setYaw(float yaw)
{
	if (entity)
	{
		auto transformComp = entity->findComponent<TransformComponent>();
		if (transformComp)
		{
			transformComp->setYaw(yaw);
		}
	}
}

float MovementComponent::getYaw() const
{
	if (entity)
	{
		auto transformComp = entity->findComponent<TransformComponent>();
		if (transformComp)
		{
			return transformComp->getYaw();
		}
	}

	return 0.0f;
}

void MovementComponent::setMovementForward(float value)
{
	velocity.z = value;
}

void MovementComponent::setMovementSideways(float value)
{
	velocity.x = value;
}

void MovementComponent::addHorizontalLook(float value)
{
	float movement = value * sensivity;

	if (entity)
	{
		auto transformComp = entity->findComponent<TransformComponent>();
		if (transformComp)
		{
			transformComp->setYaw(transformComp->getYaw() + movement);
		}
	}
}

void MovementComponent::addVerticalLook(float value)
{
	float movement = value * sensivity;

	if (entity)
	{
		auto transformComp = entity->findComponent<TransformComponent>();
		if (transformComp)
		{
			transformComp->setPitch(transformComp->getPitch() + movement);
		}
	}
}
