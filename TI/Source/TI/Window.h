#pragma once

#include <string>

#include <glm/glm.hpp>

struct SDL_Window;

class Window
{
public:
	Window();
	~Window();

	Window(const Window&) = delete;
	Window(Window&&) = delete;
	Window& operator=(const Window&) = delete;

	void init(const std::string& title, glm::ivec2 size, glm::ivec2 pos = { -1, -1 });
	void init(const std::string& title, glm::ivec2 pos = { -1, -1 });

	SDL_Window* const getSdlWindow() const;

	void swap();

	glm::ivec2 getSize() const;

	bool hasFocus() const;

	const int DEFAULT_WINDOW_WIDTH;
	const int DEFAULT_WINDOW_HEIGHT;

private:
	SDL_Window* sdlWindow;

	std::string title;
};
