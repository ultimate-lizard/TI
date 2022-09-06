#pragma once

#include <unordered_map>
#include <memory>
#include <mutex>

#include <glm/glm.hpp>

#include <TI/Client/Client.h>
#include <TI/Util/Config.h>
#include <TI/Renderer/Material.h>
#include <TI/Renderer/RenderCommand.h>
#include <TI/Client/ChunkPool.h>

class InputHandler;
class IController;
class Mesh;
class ChunkMesh;
class Renderer;
class DebugInformation;
class Plane;
class Camera;
class PlanetMesh;

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

	static DebugInformation* getDebugInformation();

	void updateBlock(const glm::uvec3& position);

	void setFrustumCullingEnabled(bool enabled);
	bool isFrustumCullingEnabled() const;

private:
	void loadConfig();
	void loadMappings();

	void renderDebugMeshes();
	void renderWorld();
	void renderEntities();

	std::vector<glm::vec3> getSurroundingChunksPositions(const glm::vec3& position, unsigned short viewDistance);
	std::vector<glm::ivec3> cullChunksPositions(const std::vector<glm::ivec3>& chunksPositions);

private:
	std::unique_ptr<InputHandler> inputHandler;
	std::unique_ptr<IController> playerController;

	Config config;

	unsigned int viewportId;

	Renderer* renderer;

	std::unordered_map<size_t, std::unique_ptr<ChunkMesh>> chunkMeshesBank;
	std::mutex meshesBankMutex;
	std::vector<size_t> pendingChunks;
	std::vector<glm::vec3> cachedVisibleChunksPositions;

	Material* chunkMaterial;

	static std::unique_ptr<DebugInformation> debugInformation;

	const Plane* plane;

	glm::ivec3 playerLastChunkPosition;

	ChunkPool pool;
	MultiDrawData cachedPoolData;

	bool frustumCullingEnabled;

	Camera* cachedEntityCamera;

	std::vector<std::unique_ptr<PlanetMesh>> stars;

	std::vector<std::unique_ptr<PlanetMesh>> planets;

	// std::unique_ptr<PlanetMesh> planetLod;
};
