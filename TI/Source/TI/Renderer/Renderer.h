#pragma once

#include <SDL.h>

class Renderer
{
public:
	Renderer(SDL_Window* sdlWindow);
	~Renderer();

	void render();

private:
	SDL_GLContext glContext;
};
