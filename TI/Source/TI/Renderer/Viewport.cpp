#include "Viewport.h"

#include <glad/glad.h>

#include <TI/Renderer/Camera.h>

Viewport::Viewport(glm::ivec2 position, glm::ivec2 size) :
	position(position),
	size(size),
	camera(nullptr),
	enabled(false)
{
	updateCameraPerspective();
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

void Viewport::setActiveCamera(Camera* camera)
{
	this->camera = camera;

	updateCameraPerspective();
}

Camera* Viewport::getActiveCamera()
{
	return camera;
}

void Viewport::setEnabled(bool enabled)
{
	this->enabled = enabled;
}

bool Viewport::isEnabled() const
{
	return enabled == true;
}

void Viewport::updateCameraPerspective()
{
	if (camera)
	{
		camera->setPerspective(
			glm::radians(95.0f),
			static_cast<float>(size.x) / static_cast<float>(size.y),
			0.01f,
			1000.0f
		);
	}
}
