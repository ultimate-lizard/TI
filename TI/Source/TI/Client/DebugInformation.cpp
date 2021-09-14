#include "DebugInformation.h"

#include <functional>

#include <glm/gtx/hash.hpp>

#include <TI/Renderer/Mesh.h>
#include <TI/Renderer/Renderer.h>
#include <TI/ResourceManager.h>
#include <TI/Client/LocalClient.h>

void drawDebugLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color, float width, bool persistent)
{
	LocalClient::getDebugInformation()->drawDebugLine(start, end, color, width, persistent);
}

void drawDebugPoint(const glm::vec3& position, const glm::vec4& color, float width, bool persistent)
{
	LocalClient::getDebugInformation()->drawDebugPoint(position, color, width, persistent);
}

void drawDebugBox(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color, float width, bool persistent)
{
	LocalClient::getDebugInformation()->drawDebugBox(position, size, color, width, persistent);
}

DebugInformation::DebugInformation(ResourceManager* resourceManager, Renderer* renderer) :
	shader(nullptr),
	resourceManager(resourceManager),
	renderer(renderer)
{
	shader = resourceManager->getShader("Default");
}

void DebugInformation::drawDebugLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color, float width, bool persistent)
{
	Material* material = nullptr;
	size_t hash = std::hash<glm::vec4>{}(color);

	auto materialIter = materials.find(hash);
	if (materialIter == materials.end())
	{
		material = materials.emplace(hash, std::make_unique<Material>(shader, nullptr, color)).first->second.get();
	}
	else
	{
		material = materialIter->second.get();
	}

	MeshBuilder meshBuilder;
	meshBuilder.setPositions({ start, end });

	meshes.push_back({ std::move(meshBuilder.build()), material, RenderMode::Lines, persistent });
}

void DebugInformation::drawDebugPoint(const glm::vec3& position, const glm::vec4& color, float width, bool persistent)
{
	Material* material = nullptr;
	size_t hash = std::hash<glm::vec4>{}(color);

	auto materialIter = materials.find(hash);
	if (materialIter == materials.end())
	{
		material = materials.emplace(hash, std::make_unique<Material>(shader, nullptr, color)).first->second.get();
	}
	else
	{
		material = materialIter->second.get();
	}

	MeshBuilder meshBuilder;
	meshBuilder.setPositions({ position });

	meshes.push_back({ std::move(meshBuilder.build()), material, RenderMode::Points, persistent, width });
}

void DebugInformation::drawDebugBox(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color, float width, bool persistent)
{
	Material* material = nullptr;
	size_t hash = std::hash<glm::vec4>{}(color);

	auto materialIter = materials.find(hash);
	if (materialIter == materials.end())
	{
		material = materials.emplace(hash, std::make_unique<Material>(shader, nullptr, color)).first->second.get();
	}
	else
	{
		material = materialIter->second.get();
	}

	MeshBuilder meshBuilder;

	meshBuilder.setPositions({
		{ position.x + size.x, position.y + size.y, position.z + size.z },
		{ position.x + 0.0f, position.y + size.y, position.z + size.z },
		{ position.x + 0.0f, position.y + 0.0f, position.z + size.z },
		{ position.x + size.x, position.y + 0.0f, position.z + size.z },
		{ position.x + 0.0f, position.y + size.y, position.z + 0.0f },
		{ position.x + size.x, position.y + size.y, position.z + 0.0f },
		{ position.x + size.x, position.y + 0.0f, position.z + 0.0f },
		{ position.x + 0.0f, position.y + 0.0f, position.z + 0.0f },
		{ position.x + 0.0f, position.y + size.y, position.z + size.z },
		{ position.x + 0.0f, position.y + size.y, position.z + 0.0f },
		{ position.x + 0.0f, position.y + 0.0f, position.z + 0.0f },
		{ position.x + 0.0f, position.y + 0.0f, position.z + size.z },
		{ position.x + size.x, position.y + size.y, position.z + 0.0f },
		{ position.x + size.x, position.y + size.y, position.z + size.z },
		{ position.x + size.x, position.y + 0.0f, position.z + size.z },
		{ position.x + size.x, position.y + 0.0f, position.z + 0.0f },
		{ position.x + size.x, position.y + size.y, position.z + size.z },
		{ position.x + size.x, position.y + size.y, position.z + 0.0f },
		{ position.x + 0.0f, position.y + size.y, position.z + 0.0f },
		{ position.x + 0.0f, position.y + size.y, position.z + size.z },
		{ position.x + size.x, position.y + 0.0f, position.z + 0.0f },
		{ position.x + size.x, position.y + 0.0f, position.z + size.z },
		{ position.x + 0.0f, position.y + 0.0f, position.z + size.z },
		{ position.x + 0.0f, position.y + 0.0f, position.z + 0.0f },
		});

	meshBuilder.setIndices({
		0, 1, 2,
		0, 2, 3,
		4, 5, 6,
		4, 6, 7,
		8, 9, 10,
		8, 10, 11,
		12, 13, 14,
		12, 14, 15,
		16, 17, 18,
		16, 18, 19,
		20, 21, 22,
		20, 22, 23
		});

	meshes.push_back({ std::move(meshBuilder.build()), material, RenderMode::Triangles, persistent });
}

void DebugInformation::updateDebugMeshLifetime()
{
	for (size_t i = 0; i < meshes.size(); ++i)
	{
		if (!meshes[i].persistent)
		{
			meshes.erase(meshes.begin() + i);
			--i;
		}
	}
}

std::vector<DebugMeshInfo>& DebugInformation::getMeshes()
{
	return meshes;
}
