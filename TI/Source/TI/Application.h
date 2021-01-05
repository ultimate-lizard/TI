#pragma once

#include <memory>

class Renderer;

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
};
