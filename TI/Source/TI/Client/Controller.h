#pragma once

#include <TI/Server/Entity.h>

class InputHandler;
class MovementComponent;
class Client;

class IController
{
public:
	IController() : entity(nullptr) {}
	virtual ~IController() = default;

	virtual void posses(Entity* entity) = 0;

	Entity* entity;
};

class PlayerController : public IController
{
public:
	PlayerController(Client* client, InputHandler* inputHandler);

	void posses(Entity* entity) override;

private:
	void setupInputHandler();

	void handleMovementForward(float value);
	void handleMovementSideways(float value);

	void handleLookVertical(float value);
	void handleLookHorizontal(float value);

	void handleLookVerticalRate(float value);
	void handleLookHorizontalRate(float value);

	void quitGame();
	void releaseMouse();

	void spawnRandomBlock();

	InputHandler* inputHandler;
	MovementComponent* movementComp;
	Client* client;
};
