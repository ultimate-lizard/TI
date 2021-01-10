#pragma once

class InputHandler;
class Camera;

class IController
{
public:
	virtual ~IController() = default;

	virtual void possesCamera(Camera* camera) = 0;
};

class PlayerController : public IController
{
public:
	PlayerController(InputHandler* inputHandler);

	void handleMovementForward(float value);
	void handleMovementSideways(float value);

	void handleLookVertical(float value);
	void handleLookHorizontal(float value);

	void handleLookVerticalRate(float value);
	void handleLookHorizontalRate(float value);

	void possesCamera(Camera* camera) override;

	void quitGame();

private:
	InputHandler* inputHandler;

	Camera* camera;

	void setupInputHandler();
};
