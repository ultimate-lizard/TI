#include "Renderer.h"

#include <stdexcept>

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include <TI/Renderer/Mesh.h>
#include <TI/Renderer/Shader.h>
#include <TI/Renderer/Camera.h>

Renderer::Renderer(SDL_Window* SDLWindow) :
	glContext(nullptr)
{
	glContext = SDL_GL_CreateContext(SDLWindow);

	if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
	{
		// TODO: Handle this
		throw std::exception();
	}

	setClearColor({ 0.0f, 1.0f, 0.0f, 1.0f });
	glEnable(GL_DEPTH_TEST);
}

Renderer::~Renderer()
{
	SDL_GL_DeleteContext(glContext);
}

void Renderer::render(Mesh* mesh, Shader* shader, Camera* camera)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	camera->updateView();
	shader->use();

	const auto& projection = camera->getProjection();
	const auto& view = camera->getView();

	auto model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));

	shader->setMatrix("projection", projection);
	shader->setMatrix("view", view);
	shader->setMatrix("model", model);
	
	// 2. Iterate over render commands
	// 2.1. Set model uniform
	// 2.2. Render mesh (bind vao, either draw arrays or elemets)
	glBindVertexArray(mesh->getVAO());

	if (mesh->getIndices().empty())
	{
		glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(mesh->getPositions().size()));
	}
	else
	{
		glDrawElements(GL_TRIANGLES, static_cast<int>(mesh->getIndices().size()), GL_UNSIGNED_INT, 0);
	}
}

void Renderer::setClearColor(glm::vec4 color)
{
	glClearColor(color.x, color.y, color.z, color.w);
}
