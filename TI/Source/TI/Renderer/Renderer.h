#pragma once

#include <list>
#include <map>

#include <glm/glm.hpp>
#include <SDL.h>

#include <TI/Renderer/FrameBuffer.h>
#include <TI/Renderer/Mesh.h>
#include <TI/Renderer/Shader.h>
#include <TI/Renderer/Viewport.h>

class Shader;
class Camera;
class Model;
class Material;
class MeshComponent;

#include <TI/Renderer/RenderCommand.h>

class Renderer
{
public:
	Renderer(SDL_Window* sdlWindow);
	~Renderer();

	void pushRender(Mesh* mesh, Material* material, const glm::mat4& transform, int viewportId = 0);
	void pushRender(MeshComponent* meshComponent, int viewportId = 0);

	void render();

	void setClearColor(const glm::vec4& color);
	const glm::vec4& getClearColor() const;

	void setCamera(Camera* camera);
	
	void createViewport(unsigned int id, glm::ivec2 pos = { 0, 0 }, glm::ivec2 size = { 0, 0 });
	Viewport* getViewport(unsigned int viewportId);

private:
	SDL_GLContext glContext;

	glm::vec4 clearColor;

	Camera* camera;

	// Viewport ID, Viewport
	std::map<int, Viewport> viewportsMap;

	// FrameBuffer framebuffer;
	// Mesh screenMesh;
	// Shader screenShader;
};
