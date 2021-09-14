#pragma once

#include <memory>
#include <vector>
#include <map>

#include <glm/glm.hpp>

#include <TI/Renderer/RenderCommand.h>

class Mesh;
class Material;
class Shader;
class Renderer;
class ResourceManager;

struct DebugMeshInfo
{
	std::unique_ptr<Mesh> mesh;
	Material* material = nullptr;
	RenderMode renderMode;
	bool persistent = true;
	float lineWidth = 1.0f;
};

void drawDebugLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color, float width, bool persistent = true);
void drawDebugPoint(const glm::vec3& position, const glm::vec4& color, float width, bool persistent = true);
void drawDebugBox(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color, float width, bool persistent = true);

class DebugInformation
{
	friend class LocalClient;

public:
	DebugInformation(const DebugInformation&) = delete;
	DebugInformation(DebugInformation&&) = delete;

	void drawDebugLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color, float width, bool persistent = true);
	void drawDebugPoint(const glm::vec3& position, const glm::vec4& color, float width, bool persistent = true);
	void drawDebugBox(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color, float width, bool persistent = true);

	void updateDebugMeshLifetime();

	std::vector<DebugMeshInfo>& getMeshes();

private:
	DebugInformation(ResourceManager* resourceManager, Renderer* renderer);
	
	Shader* shader;
	std::vector<DebugMeshInfo> meshes;
	std::map<size_t, std::unique_ptr<Material>> materials;

	ResourceManager* resourceManager;
	Renderer* renderer;
};
