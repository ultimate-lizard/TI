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

	void setClearColor(const glm::vec4& color);
	const glm::vec4& getClearColor() const;

private:
	SDL_GLContext glContext;

	glm::vec4 clearColor;
};
