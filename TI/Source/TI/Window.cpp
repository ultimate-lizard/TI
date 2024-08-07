#include "Window.h"

#include <stdexcept>

#include <SDL.h>

Window::Window()
	:
	DEFAULT_WINDOW_WIDTH(1024),
	DEFAULT_WINDOW_HEIGHT(768),
	sdlWindow(nullptr)
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

glm::ivec2 Window::getSize() const
{
	int x = 0;
	int y = 0;

	SDL_GetWindowSize(sdlWindow, &x, &y);

	return glm::ivec2(x, y);
}

bool Window::hasFocus() const
{
	int flags = SDL_GetWindowFlags(sdlWindow);

	return flags & SDL_WINDOW_INPUT_FOCUS;
}

void Window::setTitle(const std::string& newTitle)
{
	title = newTitle;
	SDL_SetWindowTitle(sdlWindow, newTitle.c_str());
}

void Window::maximize()
{
	SDL_MaximizeWindow(sdlWindow);
}
