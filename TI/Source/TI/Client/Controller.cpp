#include "Controller.h"

#include <iostream>

#include <SDL.h>

#include <TI/Client/InputHandler.h>
#include <TI/Renderer/Camera.h>
#include <TI/Server/Entity.h>
#include <TI/Server/Component/MovementComponent.h>

// A number to multiply camera sensivity during controller camera
// vertical and horizontal movements to match the sensivity of the mouse input
static const float CONTROLLER_SENSIVITY_ADJUSTER = 100.0f;

PlayerController::PlayerController(InputHandler* inputHandler) :
	inputHandler(inputHandler),
	entity(nullptr),
	movementComp(nullptr)
{
	setupInputHandler();
}

void PlayerController::handleMovementForward(float value)
{
	if (movementComp)
	{
		movementComp->setMovementForward(value);
	}
}

void PlayerController::handleMovementSideways(float value)
{
	if (movementComp)
	{
		movementComp->setMovementSideways(value);
	}
}

void PlayerController::handleLookVertical(float value)
{
	if (movementComp)
	{
		movementComp->addVerticalLook(value);
	}
}

void PlayerController::handleLookHorizontal(float value)
{
	if (movementComp)
	{
		movementComp->addHorizontalLook(value);
	}
}

void PlayerController::handleLookVerticalRate(float value)
{
	if (movementComp)
	{
		movementComp->setPitchRate(value * CONTROLLER_SENSIVITY_ADJUSTER);
	}
}

void PlayerController::handleLookHorizontalRate(float value)
{
	if (movementComp)
	{
		movementComp->setYawRate(value * CONTROLLER_SENSIVITY_ADJUSTER);
	}
}


void PlayerController::posses(Entity* entity)
{
	this->entity = entity;
	if (entity)
	{
		movementComp = entity->findComponent<MovementComponent>();
	}
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
