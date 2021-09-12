#pragma once

#include <vector>

#include <glm/glm.hpp>

#include <TI/Client/Client.h>
#include <TI/Util/Config.h>

class InputHandler;
class IController;
class Mesh;
class Material;
class ChunkMesh;

struct DebugMeshInfo
{
	std::string name;
	glm::vec4 color;
	float width;
	int meshType;
	bool wireframe = true;
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

	void drawDebugLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color, float width);
	void drawDebugPoint(const glm::vec3& position, const glm::vec4& color, float width);
	void drawDebugBox(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color, float width);

	// TODO: This is a temporary method
	std::vector<ChunkMesh*>& getChunkMeshes();

private:
	void loadConfig();
	void loadMappings();

	std::unique_ptr<InputHandler> inputHandler;
	std::unique_ptr<IController> playerController;

	Config config;

	unsigned int viewportId;

	std::vector<DebugMeshInfo> debugMeshes;
	// std::unique_ptr<Material> debugMeshMaterial;

	// Material* debugMaterial;

	std::vector<ChunkMesh*> chunkMeshes;
	Material* chunkMaterial;
};
