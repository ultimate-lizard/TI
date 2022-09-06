#pragma once

#include <list>
#include <array>

#include <TI/Renderer/RenderCommand.h>

class Camera;

class Viewport
{
public:
	Viewport(glm::ivec2 position = { 0, 0 }, glm::ivec2 size = { 800, 600 });

	void bind();

	void setPosition(const glm::ivec2& position);

	void setSize(const glm::ivec2& size);
	const glm::ivec2 getSize() const;

	void setActiveCamera(Camera* camera);
	Camera* getActiveCamera();

	void setEnabled(bool enabled);
	bool isEnabled() const;

	std::array<std::list<RenderCommand>, 3> passes;
	// std::list<RenderCommand> renderCommands;

private:
	void updateCameraPerspective();

private:
	glm::ivec2 position;
	glm::ivec2 size;
	Camera* camera;

	bool enabled;
};
