#include "Renderer.h"

#include <stdexcept>
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

#include <TI/Renderer/Mesh.h>
#include <TI/Renderer/Model.h>
#include <TI/Renderer/Shader.h>
#include <TI/Renderer/Camera.h>
#include <TI/Renderer/Texture.h>
#include <TI/Renderer/Material.h>
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

	// setClearColor({ 0.4f, 0.8f, 1.0f, 1.0f });
	setClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	SDL_GL_SetSwapInterval(1);

	createDefaultViewport(window);
}

Renderer::~Renderer()
{
	SDL_GL_DeleteContext(glContext);
}

void Renderer::pushRender(RenderCommand command)
{
	if (auto iter = viewportsMap.find(command.viewportId); iter != viewportsMap.end())
	{
		if (!iter->second.isEnabled()) return;

		renderCommands.push_front(std::move(command));
	}
}

void Renderer::render()
{
	/*glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);*/

	for (auto& viewportIter : viewportsMap)
	{
		Viewport& viewport = viewportIter.second;

		viewport.bind();

		Camera* camera = viewport.getActiveCamera();
		if (!camera)
		{
			continue;
		}

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

				shader->setVector4("color", command.material->getColor());
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
			int renderMode = static_cast<int>(command.renderMode);
			if (mesh->getIndicesCount())
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->getEBO());
				glDrawElements(renderMode, static_cast<unsigned int>(mesh->getIndicesCount()), GL_UNSIGNED_INT, 0);
			}
			else
			{
				glDrawArrays(renderMode, 0, static_cast<unsigned int>(mesh->getPositionsCount()));
			}
		}
	}
}

void Renderer::renderMultidraw(Mesh* mesh, Material* material, GLsizei* counts, void** indices, GLsizei drawCount)
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

		if (!mesh)
		{
			return;
		}

		if (material)
		{
			if (Texture* texture = material->getTexture())
			{
				texture->bind();
			}

			Shader* shader = material->getShader();
			// assert
			shader->use();

			const glm::mat4& projection = camera->getProjection();
			const glm::mat4& view = camera->getView();

			shader->setMatrix("projection", projection);
			shader->setMatrix("view", view);
			shader->setMatrix("model", glm::mat4(1.0f));

			shader->setVector4("color", material->getColor());
		}

		glBindVertexArray(mesh->getVAO());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->getEBO());

		// int renderMode = static_cast<int>(command.renderMode);
		glMultiDrawElements(GL_TRIANGLES, counts, GL_UNSIGNED_INT, indices, drawCount);
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
