#include "Window.h"

#include <stdexcept>

#include <SDL.h>

Window::Window()
	:
	DEFAULT_WINDOW_WIDTH(1024),
	DEFAULT_WINDOW_HEIGHT(768),
	sdlWindow(nullptr),
	size({DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT})
{

}

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
		| SDL_WINDOW_RESIZABLE
	);

	if (!sdlWindow)
	{
		// TODO: Handle this
		throw std::exception();
	}
}

void Window::init(const std::string& title, glm::ivec2 pos /*= { -1, -1 }*/)
{
	init(title, { DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT }, pos);
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

bool Window::hasFocus() const
{
	int flags = SDL_GetWindowFlags(sdlWindow);

	return flags & SDL_WINDOW_INPUT_FOCUS;
}
