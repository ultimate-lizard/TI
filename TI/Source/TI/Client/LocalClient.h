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
class DebugInformation;

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

	// This is a temporary method
	std::vector<ChunkMesh*>& getChunkMeshes();

	static DebugInformation* getDebugInformation();

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

	std::vector<ChunkMesh*> chunkMeshes;
	Material* chunkMaterial;

	static std::unique_ptr<DebugInformation> debugInformation;
};
