#pragma once

#include <SDL.h>
#include <glm/glm.hpp>

class Mesh;
class Shader;
class Camera;

class Renderer
{
public:
	Renderer(SDL_Window* sdlWindow);
	~Renderer();

	void render(Mesh* mesh, Shader* shader, Camera* camera);

	void setClearColor(glm::vec4 color);

private:
	SDL_GLContext glContext;
};
