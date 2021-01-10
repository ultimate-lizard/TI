#include "Controller.h"

#include <iostream>

#include <SDL.h>

#include <TI/Client/InputHandler.h>
#include <TI/Renderer/Camera.h>

// A number to multiply on camera sensivity during controller camera
// vertical and horizontal movements to match the sensivity of the mouse input
static const float CONTROLLER_SENSIVITY_ADJUSTER = 100.0f;

PlayerController::PlayerController(InputHandler* inputHandler) :
	inputHandler(inputHandler)
{
	setupInputHandler();
}

void PlayerController::handleMovementForward(float value)
{
	if (camera)
	{
		glm::vec3 velocity = camera->getVelocity();

		velocity.z = value;

		camera->setVelocity(velocity);
	}
}

void PlayerController::handleMovementSideways(float value)
{
	if (camera)
	{
		glm::vec3 velocity = camera->getVelocity();

		velocity.x = value;

		camera->setVelocity(velocity);
	}
}

void PlayerController::handleLookVertical(float value)
{
	float movement = value * camera->getSensivity();
	camera->setPitch(camera->getPitch() + movement);
}

void PlayerController::handleLookHorizontal(float value)
{
	float movement = value * camera->getSensivity();
	camera->setYaw(camera->getYaw() + movement);
}

void PlayerController::handleLookVerticalRate(float value)
{
	camera->pitchRate = value * camera->getSensivity() * CONTROLLER_SENSIVITY_ADJUSTER;
}

void PlayerController::handleLookHorizontalRate(float value)
{
	camera->yawRate = value * camera->getSensivity() * CONTROLLER_SENSIVITY_ADJUSTER;
}

void PlayerController::possesCamera(Camera* camera)
{
	this->camera = camera;
}

void PlayerController::quitGame()
{
	exit(0);
}

void PlayerController::setupInputHandler()
{
	if (inputHandler)
	{
		inputHandler->bindAxis("ForwardMovementAxis", std::bind(&PlayerController::handleMovementForward, this, std::placeholders::_1));
		inputHandler->bindAxis("SideMovementAxis", std::bind(&PlayerController::handleMovementSideways, this, std::placeholders::_1));

		inputHandler->bindAxis("HorizontalLook", std::bind(&PlayerController::handleLookHorizontal, this, std::placeholders::_1));
		inputHandler->bindAxis("VerticalLook", std::bind(&PlayerController::handleLookVertical, this, std::placeholders::_1));

		inputHandler->bindAxis("HorizontalLookRate", std::bind(&PlayerController::handleLookHorizontalRate, this, std::placeholders::_1));
		inputHandler->bindAxis("VerticalLookRate", std::bind(&PlayerController::handleLookVerticalRate, this, std::placeholders::_1));

		inputHandler->bindKey("QuitGame", ActionInputType::KeyPress, std::bind(&PlayerController::quitGame, this));
	}
}
