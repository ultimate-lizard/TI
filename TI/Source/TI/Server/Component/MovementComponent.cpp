#include "MovementComponent.h"

#include <iostream>

#include <TI/Server/Entity.h>
#include <TI/Server/Component/CameraComponent.h>
#include <TI/Renderer/Camera.h>
#include <TI/Server/Component/TransformComponent.h>
#include <TI/Server/Plane.h>
#include <TI/Client/LocalClient.h>

float maxVelocity = 100.0f;

MovementComponent::MovementComponent(TransformComponent* const transformComponent) :
	transformComponent(transformComponent),
	yawRate(0.0f),
	pitchRate(0.0f),
	speed(70.0f),
	forward(0.0f, 0.0f, -1.0f),
	up({ 0.0f, 1.0f, 0.0f }),
	right({ 1.0f, 0.0f, 0.0f }),
	sensivity(20.0f)
{

}

MovementComponent::MovementComponent(const MovementComponent& otherMovementComp)
{
	transformComponent = otherMovementComp.transformComponent;

	yawRate = otherMovementComp.yawRate;
	pitchRate = otherMovementComp.pitchRate;

	velocity = otherMovementComp.velocity;
	speed = otherMovementComp.speed;

	forward = otherMovementComp.forward;
	up = otherMovementComp.up;
	right = otherMovementComp.right;

	sensivity = otherMovementComp.sensivity;
}

void MovementComponent::tick(float dt)
{
	if (!transformComponent)
	{
		return;
	}

	glm::vec3 rotation = transformComponent->getRotation();

	rotation.x += pitchRate * dt;
	rotation.y += yawRate * dt;

	if (rotation.x > 89.0f)
	{
		rotation.x = 89.0f;
	}
	if (rotation.x < -89.0f)
	{
		rotation.x = -89.0f;
	}

	glm::vec3 newForward;
	newForward.x = cos(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));
	newForward.y = sin(glm::radians(rotation.x));
	newForward.z = sin(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));

	forward = glm::normalize(newForward);

	right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
	up = glm::normalize(glm::cross(right, forward));

	glm::vec3 position = transformComponent->getPosition();

	// COLLISION DETECTION
	glm::vec3 playerMin = position;
	playerMin.x -= 0.5f;
	playerMin.y -= 1.8f;
	playerMin.z -= 0.5f;

	glm::vec3 playerMax = position;
	playerMax.x += 0.5f;
	playerMax.y += 0.2f;
	playerMax.z += 0.5f;

	std::vector<std::pair<glm::uvec3, glm::uvec3>> blocksAround;

	const Plane* plane = transformComponent->getPlane();

	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -2; y <= 2; ++y)
		{
			for (int z = -1; z <= 1; ++z)
			{
				glm::vec3 playerPos = position;
				playerPos.x += x;
				playerPos.y += y;
				playerPos.z += z;

				if (plane->getBlock(playerPos) != 0)
				{
					glm::uvec3 blockMin;
					glm::uvec3 blockMax;

					blockMin = playerPos;
					blockMax = playerPos;

					blockMax.x += 1;
					blockMax.y += 1;
					blockMax.z += 1;

					// entity->getLocalClient()->drawDebugLine(blockMin, blockMax, { 0.0f, 1.0f, 0.0f, 1.0f }, 5.0f);

					blocksAround.push_back({ blockMin, blockMax });
				}
			}
		}
	}

	bool collidedX = false;
	bool collidedY = false;
	bool collidedZ = false;
	for (const auto& pair : blocksAround)
	{
		const glm::uvec3 blockMin = pair.first;
		const glm::uvec3 blockMax = pair.second;

		if (playerMin.y <= blockMax.y && playerMax.y >= blockMin.y)
		{
			collidedY = true;
		}

		if (playerMin.x <= blockMax.x && playerMax.x >= blockMin.x)
		{
			collidedX = true;
		}

		if (playerMin.z <= blockMax.z && playerMax.z >= blockMin.z)
		{
			collidedZ = true;
		}
	}

	position += getVelocity().z * (speed * forward) * dt;
	position += getVelocity().x * (speed * glm::cross(forward, up)) * dt;

	// GRAVITY
	static float adjuster = 0.0f;

	if (velocity.x || velocity.y || velocity.z)
	{
		adjuster = 0.0f;
	}
	else
	{
		adjuster += dt * 0.15f;
		if (adjuster > maxVelocity)
		{
			adjuster = maxVelocity;
		}

		if (collidedY)
		{
			adjuster = 0.0f;
		}
		else
		{
			// apply gravity
			position += glm::vec3(0.0f, -1.0f, 0.0f) * adjuster;
		}
	}

	transformComponent->setRotation(rotation);
	transformComponent->setPosition(position);
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

void MovementComponent::setMovementForward(float value)
{
	velocity.z = value;
}

void MovementComponent::setMovementSideways(float value)
{
	velocity.x = value;
}

std::unique_ptr<Component> MovementComponent::clone() const
{
	return std::make_unique<MovementComponent>(*this);
}

const glm::vec3& MovementComponent::getForward() const
{
	return forward;
}

void MovementComponent::operator=(const MovementComponent& otherMovementComp)
{
	yawRate = otherMovementComp.yawRate;
	pitchRate = otherMovementComp.pitchRate;

	velocity = otherMovementComp.velocity;
	float speed = otherMovementComp.speed;

	forward = otherMovementComp.forward;
	up = otherMovementComp.up;
	right = otherMovementComp.right;

	sensivity = otherMovementComp.sensivity;
}

void MovementComponent::addHorizontalLook(float value)
{
	float movement = value * sensivity;

	if (transformComponent)
	{
		glm::vec3 rotation = transformComponent->getRotation();
		rotation.y = rotation.y + movement;
		// transformComp->setYaw(transformComp->getYaw() + movement);
		transformComponent->setRotation(rotation);
	}
}

void MovementComponent::addVerticalLook(float value)
{
	float movement = value * sensivity;

	if (transformComponent)
	{
		// transformComp->setPitch(transformComp->getPitch() + movement);
		glm::vec3 rotation = transformComponent->getRotation();
		rotation.x = rotation.x + movement;
		transformComponent->setRotation(rotation);
	}
}
