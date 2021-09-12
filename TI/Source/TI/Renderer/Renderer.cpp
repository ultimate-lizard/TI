#include "Renderer.h"

#include <stdexcept>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

#include <TI/Renderer/Mesh.h>
#include <TI/Renderer/Model.h>
#include <TI/Renderer/Shader.h>
#include <TI/Renderer/Camera.h>
#include <TI/Renderer/Texture.h>
#include <TI/Renderer/Material.h>
#include <TI/Server/Component/MeshComponent.h>
#include <TI/Server/Entity.h>
#include <TI/Window.h>

Renderer::Renderer(Window* window) :
	glContext(nullptr),
	camera(nullptr),
	wireframe(false)
{
	glContext = SDL_GL_CreateContext(window->getSdlWindow());

	if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
	{
		// TODO: Handle this
		throw std::exception();
	}

	setClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });

	glEnable(GL_DEPTH_TEST);

	SDL_GL_SetSwapInterval(0);

	createDefaultViewport(window);
}

Renderer::~Renderer()
{
	SDL_GL_DeleteContext(glContext);
}

void Renderer::pushRender(Mesh* mesh, Material* material, const glm::mat4& transform, int viewportId, unsigned int renderMode, float lineWidth, bool wireframe, bool cullFaces)
{
	if (auto iter = viewportsMap.find(viewportId); iter != viewportsMap.end())
	{
		if (!iter->second.isEnabled()) return;

		iter->second.getRenderCommands().push_front({ mesh, material, transform, renderMode, lineWidth, wireframe, cullFaces });
	}
}

// Render should not work with components
// TODO: Remove this method
void Renderer::pushRender(MeshComponent* meshComponent, int viewportId, unsigned int renderMode)
{
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
		Viewport& viewport = viewportIter.second;

		viewport.bind();

		Camera* camera = viewport.getActiveCamera();
		if (!camera)
		{
			continue;
		}

		std::list<RenderCommand>& renderCommands = viewport.getRenderCommands();

		while (!renderCommands.empty())
		{
			auto command = renderCommands.back();
			renderCommands.pop_back();

			if (!command.mesh)
			{
				continue;
			}

			if (command.material)
			{
				if (Texture* texture = command.material->getTexture())
				{
					texture->bind();
				}

				Shader* shader = command.material->getShader();
				// assert
				shader->use();

				const glm::mat4& projection = camera->getProjection();
				const glm::mat4& view = camera->getView();

				shader->setMatrix("projection", projection);
				shader->setMatrix("view", view);
				shader->setMatrix("model", command.transform);

				shader->setVector("color", command.material->getColor());
			}
			
			glLineWidth(command.lineWidth);
			glPointSize(command.lineWidth);

			glPolygonMode(GL_FRONT_AND_BACK, command.wireframe || wireframe ? GL_LINE : GL_FILL);

			if (command.cullFaces)
			{
				glEnable(GL_CULL_FACE);
			}
			else
			{
				glDisable(GL_CULL_FACE);
			}

			glBindVertexArray(command.mesh->getVAO());

			Mesh* mesh = command.mesh;
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

void Renderer::setWireframe(bool flag)
{
	wireframe = flag;
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
