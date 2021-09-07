#pragma once

#include <glm/glm.hpp>

#include <TI/Client/Client.h>
#include <TI/Util/Config.h>

class InputHandler;
class IController;
class Mesh;
class Material;
class ChunkMesh;

struct DebugLine
{
	std::string name;
	glm::vec4 color;
	float width;
};

class LocalClient : public Client
{
public:
	LocalClient(Application* app, const std::string& name = "Player");

	void update(float dt) override;

	void connect(const std::string& ip, int port) override;
	void setPossesedEntity(Entity* entity) override;

	InputHandler* const getInputHandler() const;
	IController* const getController() const;

	void shutdown() override;

	void setViewportId(unsigned int id);
	unsigned int getViewportId() const;

	void drawDebugLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color, float width);

	// TODO: This is a temporary method
	ChunkMesh* getChunkMesh();

private:
	void loadConfig();
	void loadMappings();

	std::unique_ptr<InputHandler> inputHandler;
	std::unique_ptr<IController> playerController;

	Config config;

	unsigned int viewportId;

	// Debug info rendering
	// TODO: Maybe move to ModelManager?
	std::vector<DebugLine> debugMeshes;
	Material* debugMaterial;

	ChunkMesh* chunkMesh;
};
