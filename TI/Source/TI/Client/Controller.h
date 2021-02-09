#pragma once

class InputHandler;
class Entity;
class MovementComponent;

class IController
{
public:
	virtual ~IController() = default;

	virtual void posses(Entity* entity) = 0;
};

class PlayerController : public IController
{
public:
	PlayerController(InputHandler* inputHandler);

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

	InputHandler* inputHandler;
	Entity* entity;
	MovementComponent* movementComp;
};
