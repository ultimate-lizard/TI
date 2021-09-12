#pragma once

#include <list>
#include <map>

#include <glm/glm.hpp>
#include <SDL.h>

#include <TI/Renderer/FrameBuffer.h>
#include <TI/Renderer/Mesh.h>
#include <TI/Renderer/Shader.h>
#include <TI/Renderer/Viewport.h>

#include <glad/glad.h>

#include <TI/Renderer/RenderCommand.h>

class Shader;
class Camera;
class Model;
class Material;
class MeshComponent;
class Window;

class Renderer
{
public:
	Renderer(Window* window);
	~Renderer();

	void pushRender(Mesh* mesh, Material* material, const glm::mat4& transform, int viewportId = 0, unsigned int renderMode = GL_TRIANGLES, float lineWidth = 1.0f, bool wireframe = false, bool cullFaces = true);
	void pushRender(MeshComponent* meshComponent, int viewportId = 0, unsigned int renderMode = GL_TRIANGLES);

	void render();

	void setClearColor(const glm::vec4& color);
	const glm::vec4& getClearColor() const;

	void createViewport(unsigned int id, glm::ivec2 pos = { 0, 0 }, glm::ivec2 size = { 0, 0 });
	Viewport* getViewport(unsigned int viewportId);

	void setLineWidth(float width);

	void setPolygonMode(int mode);

private:
	void createDefaultViewport(Window* window);

private:
	SDL_GLContext glContext;

	glm::vec4 clearColor;

	Camera* camera;

	// Viewport ID, Viewport
	std::map<int, Viewport> viewportsMap;

	std::vector<Mesh*> debugMeshes;
};
