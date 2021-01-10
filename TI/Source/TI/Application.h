#pragma once

#include <memory>

class Renderer;
class InputHandler;
class IController;

class Application
{
public:
	Application();
	~Application();

	Application(const Application&) = delete;
	Application(Application&&) = delete;
	Application& operator=(const Application&) = delete;

	void start();

private:
	void init();

private:
	bool simulating;

	struct SDL_Window* sdlWindow;

	std::unique_ptr<Renderer> renderer;
	std::unique_ptr<InputHandler> inputHandler;
	std::unique_ptr<IController> playerController;
};
