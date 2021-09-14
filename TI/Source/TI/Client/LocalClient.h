#pragma once

#include <vector>
#include <map>
#include <memory>

#include <glm/glm.hpp>

#include <TI/Client/Client.h>
#include <TI/Util/Config.h>
#include <TI/Renderer/Material.h>
#include <TI/Renderer/RenderCommand.h>

class InputHandler;
class IController;
class Mesh;
class ChunkMesh;
class Renderer;

struct DebugMeshInfo
{
	std::unique_ptr<Mesh> mesh;
	Material* material = nullptr;
	RenderMode renderMode;
	bool persistent = true;
	float lineWidth = 1.0f;
};

class LocalClient : public Client
{
public:
	LocalClient(Application* app, const std::string& name = "Player");
	~LocalClient();

	void update(float dt) override;

	void connect(const std::string& ip, int port) override;
	void setPossesedEntity(Entity* entity) override;

	InputHandler* const getInputHandler() const;
	IController* const getController() const;

	void shutdown() override;

	void setViewportId(unsigned int id);
	unsigned int getViewportId() const;

	void drawDebugLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color, float width, bool persistent = true);
	void drawDebugPoint(const glm::vec3& position, const glm::vec4& color, float width, bool persistent = true);
	void drawDebugBox(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color, float width, bool persistent = true);

	// This is a temporary method
	std::vector<ChunkMesh*>& getChunkMeshes();

private:
	void loadConfig();
	void loadMappings();

	void renderDebugMeshes();
	void renderWorld();
	void renderEntities();

private:
	std::unique_ptr<InputHandler> inputHandler;
	std::unique_ptr<IController> playerController;

	Config config;

	unsigned int viewportId;

	Renderer* renderer;

	Shader* debugShader;
	std::vector<DebugMeshInfo> debugMeshes;
	std::map<size_t, std::unique_ptr<Material>> debugMaterials;

	std::vector<ChunkMesh*> chunkMeshes;
	Material* chunkMaterial;
};
