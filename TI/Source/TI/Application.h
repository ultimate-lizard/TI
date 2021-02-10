#pragma once

#include <memory>

// TODO: Move this to Window class
static const int DEFAULT_WINDOW_WIDTH = 800;
static const int DEFAULT_WINDOW_HEIGHT = 600;

class Renderer;
class InputHandler;
class IController;
class Input;

struct SDL_Window;

class Application
{
public:
	Application();
	~Application();

	Application(const Application&) = delete;
	Application(Application&&) = delete;
	Application& operator=(const Application&) = delete;

	void start();
	void stop();

	InputHandler* const getInputHandler() const;

	SDL_Window* const getSDLWindow() const;

private:
	void init();

private:
	bool simulating;

	SDL_Window* sdlWindow;

	std::unique_ptr<Renderer> renderer;
	std::unique_ptr<Input> input;
	std::unique_ptr<InputHandler> inputHandler;
	std::unique_ptr<IController> playerController;
};
