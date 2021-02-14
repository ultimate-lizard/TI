#include "Renderer.h"

#include <stdexcept>

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include <TI/Renderer/Mesh.h>
#include <TI/Renderer/Shader.h>
#include <TI/Renderer/Camera.h>
#include <TI/Renderer/Model.h>
#include <TI/Server/Component/MeshComponent.h>
#include <TI/Server/Component/TransformComponent.h>
#include <TI/Server/Entity.h>

Renderer::Renderer(SDL_Window* SDLWindow) :
	glContext(nullptr),
	camera(nullptr)
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

void Renderer::pushRender(Mesh* mesh, Material* material, const glm::mat4& transform)
{
	renderCommands.push_front({mesh, material, transform});
}

void Renderer::pushRender(MeshComponent* meshComponent)
{
	auto transformComp = meshComponent->getEntity()->findComponent<TransformComponent>();
	if (transformComp)
	{
		auto model = meshComponent->getModel();
		auto mesh = model->getMesh();
		auto material = model->getMaterial();
		auto transform = transformComp->getTransform();

		pushRender(mesh, material, transform);
	}
}

void Renderer::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);

	if (!camera)
	{
		return;
	}

	const auto& projection = camera->getProjection();
	const auto& view = camera->getView();

	while (!renderCommands.empty())
	{
		auto command = renderCommands.back();
		renderCommands.pop_back();

		command.material->getTexture()->bind();

		auto shader = command.material->getShader();
		shader->use();

		shader->setMatrix("projection", projection);
		shader->setMatrix("view", view);
		shader->setMatrix("model", command.transform);

		auto mesh = command.mesh;

		glBindVertexArray(command.mesh->getVAO());
		if (mesh->getIndices().empty())
		{
			glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(mesh->getPositions().size()));
		}
		else
		{
			glDrawElements(GL_TRIANGLES, static_cast<int>(mesh->getIndices().size()), GL_UNSIGNED_INT, 0);
		}
	}
}

void Renderer::setClearColor(const glm::vec4& color)
{
	clearColor = color;
}

const glm::vec4& Renderer::getClearColor() const
{
	return clearColor;
}

void Renderer::setCamera(Camera* camera)
{
	this->camera = camera;
}
