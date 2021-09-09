#include "Renderer.h"

#include <stdexcept>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

#include <TI/Renderer/Mesh.h>
#include <TI/Renderer/Shader.h>
#include <TI/Renderer/Camera.h>
#include <TI/Renderer/Model.h>
#include <TI/Server/Component/MeshComponent.h>
#include <TI/Server/Entity.h>
#include <TI/Window.h>

Renderer::Renderer(Window* window) :
	glContext(nullptr),
	camera(nullptr)
{
	glContext = SDL_GL_CreateContext(window->getSdlWindow());

	if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
	{
		// TODO: Handle this
		throw std::exception();
	}

	setClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	SDL_GL_SetSwapInterval(1);

	createDefaultViewport(window);
}

Renderer::~Renderer()
{
	SDL_GL_DeleteContext(glContext);
}

void Renderer::pushRender(Mesh* mesh, Material* material, const glm::mat4& transform, int viewportId, unsigned int renderMode, float lineWidth)
{
	if (auto iter = viewportsMap.find(viewportId); iter != viewportsMap.end())
	{
		if (!iter->second.isEnabled()) return;

		iter->second.getRenderCommands().push_front({ mesh, material, transform, renderMode, lineWidth });
	}
}

void Renderer::pushRender(MeshComponent* meshComponent, int viewportId, unsigned int renderMode)
{
	// Render should not work with components
	// auto transformComp = meshComponent->getEntity()->findComponent<TransformComponent>();
	auto model = meshComponent->getModel();
	auto mesh = model->getMesh();
	auto material = model->getMaterial();
	auto transform = meshComponent->getTransform();

	pushRender(mesh, material, transform, viewportId, renderMode);
}

void Renderer::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);

	for (auto& viewportIter : viewportsMap)
	{
		auto& viewport = viewportIter.second;

		viewport.bind();

		auto viewportSize = viewport.getSize();

		auto camera = viewport.getActiveCamera();
		if (!camera)
		{
			continue;
		}

		auto& renderCommands = viewport.getRenderCommands();

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

			shader->setVector("color", command.material->getColor());

			auto mesh = command.mesh;

			glLineWidth(command.lineWidth);
			glPointSize(command.lineWidth);

			glBindVertexArray(command.mesh->getVAO());
			if (mesh->getIndicesCount())
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->getEBO());
				glDrawElements(command.renderMode, static_cast<unsigned int>(mesh->getIndicesCount()), GL_UNSIGNED_INT, 0);
			}
			else
			{
				glDrawArrays(command.renderMode, 0, static_cast<unsigned int>(mesh->getPositionsCount()));
			}
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

void Renderer::createViewport(unsigned int id, glm::ivec2 pos /*= { 0, 0 }*/, glm::ivec2 size /*= { 0, 0 }*/)
{
	if (auto iter = viewportsMap.find(id); iter == viewportsMap.end())
	{
		if (!size.x || !size.y)
		{
			viewportsMap.emplace(id, Viewport(pos));
		}
		else
		{
			viewportsMap.emplace(id, Viewport(pos, size));
		}
	}
}

Viewport* Renderer::getViewport(unsigned int id)
{
	if (auto iter = viewportsMap.find(id); iter != viewportsMap.end())
	{
		return &iter->second;
	}

	return nullptr;
}

void Renderer::setLineWidth(float width)
{
	glLineWidth(width);
}

void Renderer::setPolygonMode(int mode)
{
	glPolygonMode(GL_FRONT, mode);
}

void Renderer::createDefaultViewport(Window* window)
{
	if (window)
	{
		Viewport viewport;

		viewport.setEnabled(true);

		glm::ivec2 windowSize = window->getSize();
		viewport.setSize(windowSize);

		viewportsMap.emplace(0, std::move(viewport));
	}
}
