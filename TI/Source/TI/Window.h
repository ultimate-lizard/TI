#pragma once

#include <string>

#include <glm/glm.hpp>

struct SDL_Window;

class Window
{
public:
	~Window();

	void init(const std::string& title, glm::ivec2 size = { 800, 600 }, glm::ivec2 pos = { -1, -1 });

	SDL_Window* const getSdlWindow() const;

	void swap();

	const glm::ivec2& getSize() const;

private:
	SDL_Window* sdlWindow;

	std::string title;

	glm::ivec2 size;
};
