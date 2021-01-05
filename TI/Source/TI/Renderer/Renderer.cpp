#include "Renderer.h"

#include <stdexcept>

#include <glad/glad.h>

Renderer::Renderer(SDL_Window* SDLWindow) :
	glContext(nullptr)
{
	glContext = SDL_GL_CreateContext(SDLWindow);

	if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
	{
		// TODO: Handle this
		throw std::exception();
	}

	glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
}

Renderer::~Renderer()
{
	SDL_GL_DeleteContext(glContext);
}

void Renderer::render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	// glDrawArrays(GL_TRIANGLES, 0, 3);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
