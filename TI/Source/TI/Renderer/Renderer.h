#pragma once

#include <SDL.h>
#include <list>

#include <glm/glm.hpp>

class Mesh;
class Shader;
class Camera;
class Model;
class Material;
class MeshComponent;

class Renderer
{
	struct RenderCommand
	{
		Mesh* mesh;
		Material* material;
		glm::mat4 transform;
	};

public:
	Renderer(SDL_Window* sdlWindow);
	~Renderer();

	void pushRender(Mesh* mesh, Material* material, const glm::mat4& transform);
	void pushRender(MeshComponent* meshComponent);

	void render();

	void setClearColor(const glm::vec4& color);
	const glm::vec4& getClearColor() const;

	void setCamera(Camera* camera);

private:
	SDL_GLContext glContext;

	glm::vec4 clearColor;

	Camera* camera;

	std::list<RenderCommand> renderCommands;
};
