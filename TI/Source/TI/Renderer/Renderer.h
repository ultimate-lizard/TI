#pragma once

#include <map>
#include <vector>

#include <glm/glm.hpp>
#include <SDL.h>
#include <glad/glad.h>

#include <TI/Renderer/Viewport.h>
#include <TI/Renderer/RenderCommand.h>

class Camera;
class Material;
class MeshComponent;
class Window;

class Renderer
{
public:
	Renderer(Window* window);
	~Renderer();

	void pushRender(RenderCommand command, int pass = 0);

	void render();
	// void renderMultidraw(Mesh* mesh, Material* material, GLsizei* counts, void** indices, GLsizei drawCount);

	void setClearColor(const glm::vec4& color);
	const glm::vec4& getClearColor() const;

	void createViewport(unsigned int id, glm::ivec2 pos = { 0, 0 }, glm::ivec2 size = { 0, 0 });
	Viewport* getViewport(unsigned int viewportId);

	void setLineWidth(float width);

	void setWireframe(bool flag);

private:
	void createDefaultViewport(Window* window);

private:
	SDL_GLContext glContext;

	glm::vec4 clearColor;

	Camera* camera;

	// Viewport ID, Viewport
	std::map<int, Viewport> viewportsMap;

	std::vector<Mesh*> debugMeshes;

	bool wireframe;
};
