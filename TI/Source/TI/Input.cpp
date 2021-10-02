#include "Input.h"

#include <TI/Application.h>
#include <TI/Client/Input/InputHandler.h>
#include <TI/Client/LocalClient.h>
#include <TI/Window.h>
// #include <TI/Renderer/Viewport.h>
#include <TI/Renderer/Renderer.h>

Input::Input(Application* app) :
	app(app),
	event(),
	lastX(0),
	lastY(0),
	mouseReleased(false)
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
				inputHandler->onKeyInput(static_cast<Key>(event.cbutton.button), ActionInputType::Press);
				inputHandler->onAxisInput(static_cast<Axis>(event.cbutton.button), 1.0f);
				break;

			case SDL_KEYDOWN:
				inputHandler->onKeyInput(static_cast<Key>(event.key.keysym.scancode), ActionInputType::Press);
				inputHandler->onAxisInput(static_cast<Axis>(event.key.keysym.scancode), 1.0f);
				break;

			case SDL_CONTROLLERBUTTONUP:
				inputHandler->onKeyInput(static_cast<Key>(event.cbutton.button), ActionInputType::Release);
				inputHandler->onAxisInput(static_cast<Axis>(event.cbutton.button), 0.0f);
				break;

			case SDL_KEYUP:
				inputHandler->onKeyInput(static_cast<Key>(event.key.keysym.scancode), ActionInputType::Release);
				inputHandler->onAxisInput(static_cast<Axis>(event.key.keysym.scancode), 0.0f);
				break;

			case SDL_MOUSEMOTION:
				handleMouseMotion(inputHandler);
				break;

			case SDL_MOUSEBUTTONDOWN:
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
					inputHandler->onKeyInput(Key::MouseLeft, ActionInputType::Press);
					handleWindowFocus(true);
					break;
				case SDL_BUTTON_RIGHT:
					inputHandler->onKeyInput(Key::MouseRight, ActionInputType::Press);
					break;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
					inputHandler->onKeyInput(Key::MouseLeft, ActionInputType::Release);
					break;
				case SDL_BUTTON_RIGHT:
					inputHandler->onKeyInput(Key::MouseRight, ActionInputType::Release);
					break;
				}
				break;

			case SDL_CONTROLLERAXISMOTION:
				handleControllerAxisMotion(inputHandler);
				break;

			case SDL_WINDOWEVENT:
				{
					switch (event.window.event)
					{
					case SDL_WINDOWEVENT_FOCUS_LOST:
						handleWindowFocus(false);
						break;

					case SDL_WINDOWEVENT_FOCUS_GAINED:
						handleWindowFocus(true);
						break;

					case SDL_WINDOWEVENT_RESIZED:
						handleWindowResized();
						app->getSplitScreenManager().displayAll();
						break;
					}
				}
			}
		}
	}
}

void Input::releaseMouse()
{
	handleWindowFocus(false);
}

void Input::handleMouseMotion(InputHandler* inputHandler)
{
	if (mouseReleased)
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

	Window* window = app->getWindow();
	SDL_Window* sdlWindow = window->getSdlWindow();
	auto windowSize = window->getSize();

	SDL_WarpMouseInWindow(sdlWindow, windowSize.x / 2, windowSize.y / 2);
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

void Input::handleWindowFocus(bool gained)
{
	SDL_bool flag = gained ? SDL_TRUE : SDL_FALSE;

	int x, y;
	SDL_GetMouseState(&x, &y);
	lastX = x;
	lastY = y;

	SDL_SetRelativeMouseMode(flag);
	SDL_CaptureMouse(flag);

	mouseReleased = gained ? false : true;
}

void Input::handleWindowResized()
{
	SDL_Surface* surface = SDL_GetWindowSurface(app->getWindow()->getSdlWindow());
	app->getRenderer()->getViewport(0)->setSize({ surface->clip_rect.w, surface->clip_rect.h });
}
