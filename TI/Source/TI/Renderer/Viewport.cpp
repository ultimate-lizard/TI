#include "Viewport.h"

#include <glad/glad.h>

#include <TI/Renderer/Camera.h>

Viewport::Viewport(glm::ivec2 position, glm::ivec2 size) :
	position(position),
	size(size),
	camera(nullptr)
{
	updateCameraPerspective();
}

void Viewport::pushCommand(RenderCommand&& command)
{
	renderCommands.push_back(std::move(command));
}

std::list<RenderCommand>& Viewport::getRenderCommands()
{
	return renderCommands;
}

void Viewport::bind()
{
	glViewport(position.x, position.y, size.x, size.y);
}

void Viewport::setPosition(const glm::ivec2& position)
{
	this->position = position;
}

void Viewport::setSize(const glm::ivec2& size)
{
	this->size = size;

	updateCameraPerspective();
}

const glm::ivec2 Viewport::getSize() const
{
	return size;
}

void Viewport::setCamera(Camera* camera)
{
	this->camera = camera;

	updateCameraPerspective();
}

Camera* Viewport::getCamera()
{
	return camera;
}

void Viewport::updateCameraPerspective()
{
	if (camera)
	{
		camera->setPerspective(
			glm::radians(85.0f),
			static_cast<float>(size.x) / static_cast<float>(size.y),
			0.01f,
			1000.0f
		);
	}
}
