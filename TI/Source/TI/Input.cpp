#include "Input.h"

#include <TI/Application.h>
#include <TI/Client/Input/InputHandler.h>
#include <TI/Client/LocalClient.h>
#include <TI/Window.h>

Input::Input(Application* app) :
	app(app),
	event(),
	lastX(0),
	lastY(0)
{
}

void Input::handleInput()
{
	while (SDL_PollEvent(&event))
	{
		if (!app)
		{
			return;
		}

		InputHandler* inputHandler = nullptr;

		auto localClients = app->getLocalClients();
		for (auto client : localClients)
		{
			inputHandler = client->getInputHandler();

			if (!inputHandler)
			{
				continue;
			}

			switch (event.type)
			{
			case SDL_QUIT:
				app->requestQuit();
				break;

			case SDL_CONTROLLERBUTTONDOWN:
			case SDL_KEYDOWN:
				inputHandler->onKeyInput(event.key.keysym.scancode, ActionInputType::KeyPress);
				inputHandler->onAxisInput(event.key.keysym.scancode, 1.0f);
				break;

			case SDL_CONTROLLERBUTTONUP:
			case SDL_KEYUP:
				inputHandler->onKeyInput(event.key.keysym.scancode, ActionInputType::KeyRelease);
				inputHandler->onAxisInput(event.key.keysym.scancode, 0.0f);
				break;

			case SDL_MOUSEMOTION:
				handleMouseMotion(inputHandler);
				break;

			case SDL_CONTROLLERAXISMOTION:
				handleControllerAxisMotion(inputHandler);
				break;

			case SDL_WINDOWEVENT:
				{
					switch (event.window.event)
					{
					case SDL_WINDOWEVENT_FOCUS_LOST:
						{
							int x, y;
							SDL_GetMouseState(&x, &y);
							lastX = x;
							lastY = y;

							SDL_SetRelativeMouseMode(SDL_FALSE);
							SDL_CaptureMouse(SDL_FALSE);
						}
						break;

					case SDL_WINDOWEVENT_FOCUS_GAINED:
						{
							int x, y;
							SDL_GetMouseState(&x, &y);
							lastX = x;
							lastY = y;

							SDL_SetRelativeMouseMode(SDL_TRUE);
							SDL_CaptureMouse(SDL_TRUE);
						}
						break;
					}
				}
			}
		}
	}
}

void Input::handleMouseMotion(InputHandler* inputHandler)
{
	if (!app->getWindow()->hasFocus())
	{
		return;
	}

	int xPos = event.motion.x;
	int yPos = event.motion.y;

	float xOffset = static_cast<float>(xPos) - lastX;
	float yOffset = static_cast<float>(lastY) - yPos;

	lastX = xPos;
	lastY = yPos;

	xOffset = static_cast<float>(xOffset) / (127 - -127) * 2;
	yOffset = static_cast<float>(yOffset) / (127 - -127) * 2;

	inputHandler->onAxisInput(Axis::MouseX, static_cast<float>(xOffset));
	inputHandler->onAxisInput(Axis::MouseY, static_cast<float>(yOffset));

	SDL_Window* window = app->getWindow()->getSdlWindow();

	SDL_WarpMouseInWindow(window, DEFAULT_WINDOW_WIDTH / 2, DEFAULT_WINDOW_HEIGHT / 2);
}

void Input::handleControllerAxisMotion(InputHandler* inputHandler)
{
	// TODO: Move that to config
	int deadZone = 3000;

	int motion = event.caxis.value;
	float normalizedMotion = 0.0f;

	if (motion < -deadZone || motion > deadZone)
	{
		normalizedMotion = static_cast<float>(motion) / (32767 - -32768) * 2;
	}

	switch (event.caxis.axis)
	{
	case SDL_CONTROLLER_AXIS_LEFTX:
		inputHandler->onAxisInput(Axis::ControllerLeftStickX, normalizedMotion);
		break;
	case SDL_CONTROLLER_AXIS_LEFTY:
		inputHandler->onAxisInput(Axis::ControllerLeftStickY, normalizedMotion);
		break;
	case SDL_CONTROLLER_AXIS_RIGHTX:
		inputHandler->onAxisInput(Axis::ControllerRightStickX, normalizedMotion);
		break;
	case SDL_CONTROLLER_AXIS_RIGHTY:
		inputHandler->onAxisInput(Axis::ControllerRightStickY, normalizedMotion);
		break;
	}
}
