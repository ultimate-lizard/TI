#include "Window.h"

#include <stdexcept>

#include <SDL.h>

Window::~Window()
{
	if (sdlWindow)
	{
		SDL_DestroyWindow(sdlWindow);
	}
}

void Window::init(const std::string& title, glm::ivec2 size /*= { 800, 600 }*/, glm::ivec2 pos /*= { -1, -1 }*/)
{
	this->title = title;
	this->size = size;

	sdlWindow = SDL_CreateWindow(
		this->title.c_str(),
		pos.x == -1 ? SDL_WINDOWPOS_UNDEFINED : pos.x,
		pos.y == -1 ? SDL_WINDOWPOS_UNDEFINED : pos.y,
		size.x,
		size.y,
		SDL_WINDOW_SHOWN
		| SDL_WINDOW_OPENGL
	);

	if (!sdlWindow)
	{
		// TODO: Handle this
		throw std::exception();
	}
}

SDL_Window* const Window::getSdlWindow() const
{
	return sdlWindow;
}

void Window::swap()
{
	SDL_GL_SwapWindow(sdlWindow);
}

const glm::ivec2& Window::getSize() const
{
	return size;
}
