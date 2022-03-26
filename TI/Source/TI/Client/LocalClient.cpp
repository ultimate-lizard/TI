#include "LocalClient.h"

#include <filesystem>

#include <glm/gtc/matrix_transform.hpp>

#include <TI/Server/Server.h>
#include <TI/Application.h>
#include <TI/Client/Input/InputHandler.h>
#include <TI/Client/Controller.h>
#include <TI/Server/Component/MeshComponent.h>
#include <TI/Server/Component/CameraComponent.h>
#include <TI/Server/Component/TransformComponent.h>
#include <TI/Renderer/Renderer.h>
#include <TI/ResourceManager.h>
#include <TI/Client/ChunkMesh.h>
#include <TI/Server/Plane.h>
#include <TI/Client/DebugInformation.h>
#include <TI/Util/Utils.h>
#include <TI/Util/Math.h>
#include <TI/Renderer/Camera.h>

std::unique_ptr<DebugInformation> LocalClient::debugInformation = nullptr;

LocalClient::LocalClient(Application* app, const std::string& name) :
	Client(app),
	viewportId(0),
	renderer(app->getRenderer()),
	chunkMaterial(app->getResourceManager()->getMaterial("Chunk")),
	plane(nullptr),
	pool(1'500'000'000, 500'000'000),
	frustumCullingEnabled(true),
	cachedEntityCamera(nullptr)
{
	// TODO: assert(app);
	DebugInformation* debugInfoPtr = new DebugInformation(app->getResourceManager(), renderer);
	debugInformation = std::unique_ptr<DebugInformation>(debugInfoPtr);

	inputHandler = std::make_unique<InputHandler>();
	playerController = std::make_unique<PlayerController>(this, inputHandler.get());

	this->name = name.empty() ? DEFAULT_PLAYER_NAME : name;

	loadConfig();
	loadMappings();
}

LocalClient::~LocalClient()
{
	for (auto& mesh : chunkMeshesBank)
	{
		if (mesh.second)
		{
			delete mesh.second;
		}
	}

	// TODO: Replace storing chunk meshes from raw pointers to smart pointers
	for (auto pair : chunkMeshesBank)
	{
		delete pair.second;
	}

	chunkMeshesBank.clear();
}

void LocalClient::connect(const std::string& ip, int port)
{
	if (Server* server = app->getCurrentServer())
	{
		server->connectClient(this, ip, port);

		plane = server->getPlane();

		app->threadPool.pushTask([]() {
			std::cout << "This is my task!" << std::endl;
		});

		//for (const Chunk& chunk: plane->getChunks())
		//{
		//	chunkMeshesBank.emplace(utils::positionToIndex(plane->positionToChunkPosition(chunk.getPosition()), plane->getSize()), new ChunkMesh(&chunk,  plane));
		//}

		//for (const auto& pair : chunkMeshesBank)
		//{
		//	pool.insertChunkMesh(plane, pair.second);
		//}

		// cachedPoolData = pool.buildMesh();

		drawDebugLine(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 1.0f);
		drawDebugLine(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 1.0f);
		drawDebugLine(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), 1.0f);
	}
}

void LocalClient::setPossesedEntity(Entity* entity)
{
	Client::setPossesedEntity(entity);

	playerController->posses(entity);

	if (entity)
	{
		if (auto cameraComp = entity->findComponent<CameraComponent>())
		{
			cachedEntityCamera = cameraComp->getCamera();

			if (Renderer* renderer = app->getRenderer())
			{
				Viewport* viewport = renderer->getViewport(viewportId);
				if (viewport)
				{
					viewport->setActiveCamera(cameraComp->getCamera());
				}
			}
		}
	}
}

void LocalClient::update(float dt)
{
	if (possessedEntity && plane)
	{
		if (auto transformComponent = possessedEntity->findComponent<TransformComponent>())
		{
			// TODO: Find a way to get the chunk an entity stands on
			glm::vec3 playerPosition = transformComponent->getPosition();
			playerPosition.y -= 3.0f;
			
			if (playerLastChunkPosition != plane->positionToChunkPosition(playerPosition))
			{
				playerLastChunkPosition = plane->positionToChunkPosition(playerPosition);

				// TODO: Pass the chunk pool a list of visible chunks?
				for (const auto& position : cachedVisibleChunksPositions)
				{
					pool.setChunkMeshVisibility(utils::positionToIndex(position, plane->getSize()), false);
				}

				const int VIEW_DISTANCE = 32;
				cachedVisibleChunksPositions = getSurroundingChunksPositions(playerPosition, VIEW_DISTANCE);

				std::cout << "EBO size: " << pool.analyticEboSize / 1024 / 1024 << " MB" << std::endl;
				std::cout << "VBO size: " << pool.analyticVboSize / 1024 / 1024 << " MB" << std::endl;
			}

			if (cachedEntityCamera)
			{
				Frustum frustum(cachedEntityCamera->getProjection() * cachedEntityCamera->getView());

				for (size_t i = 0; i < cachedVisibleChunksPositions.size(); ++i)
				{
					const glm::ivec3& chunkPosition = cachedVisibleChunksPositions[i];
					const size_t chunkSize = plane->getChunkSize();

					glm::ivec3 absoluteChunkPosition(chunkPosition.x * chunkSize, chunkPosition.y * chunkSize, chunkPosition.z * chunkSize);
					if (frustum.IsBoxVisible(absoluteChunkPosition, { absoluteChunkPosition.x + chunkSize, absoluteChunkPosition.y + chunkSize, absoluteChunkPosition.z + chunkSize }))
					{
						const size_t chunkIndex = utils::positionToIndex(chunkPosition, plane->getSize());
						// If mesh is built and not pending
						meshesBankMutex.lock();
						if (auto foundIter = chunkMeshesBank.find(chunkIndex); foundIter != chunkMeshesBank.end())
						{
							meshesBankMutex.unlock();
							if (pool.findChunkMesh(chunkIndex))
							{
								pool.setChunkMeshVisibility(chunkIndex, true);
							}
							else
							{
								pool.insertChunkMesh(plane, foundIter->second);
							}
						}
						// If mesh is not yet built
						else
						{
							meshesBankMutex.unlock();
							// If not pending, create task
							if (std::find(pendingChunks.begin(), pendingChunks.end(), chunkIndex) == pendingChunks.end())
							{
								{
									std::unique_lock<std::mutex> lock(meshesBankMutex);
									pendingChunks.push_back(chunkIndex);
								}

								app->threadPool.pushTask([this, chunkPosition, chunkIndex]() {
									{
										ChunkMesh* newMesh = new ChunkMesh(plane->getChunk(chunkPosition), plane);
										std::unique_lock<std::mutex> lock(meshesBankMutex);
										chunkMeshesBank.emplace(chunkIndex, newMesh);
										auto foundIter = std::find(pendingChunks.begin(), pendingChunks.end(), chunkIndex);
										pendingChunks.erase(foundIter);
									}
								});
							}
						}
					}
					else
					{
						pool.setChunkMeshVisibility(utils::positionToIndex(chunkPosition, plane->getSize()), false);
					}
				}

				cachedPoolData = pool.buildMesh();
			}
		}
	}

	renderDebugMeshes();
	renderWorld();
	renderEntities();
}

InputHandler* const LocalClient::getInputHandler() const
{
	return inputHandler.get();
}

IController* const LocalClient::getController() const
{
	return playerController.get();
}

void LocalClient::shutdown()
{
	config.save();

	Client::shutdown();
}

void LocalClient::setViewportId(unsigned int id)
{
	viewportId = id;
}

unsigned int LocalClient::getViewportId() const
{
	return viewportId;
}

DebugInformation* LocalClient::getDebugInformation()
{
	return debugInformation.get();
}

void LocalClient::updateBlock(const glm::uvec3& position)
{
	glm::uvec3 chunkPosition = plane->positionToChunkPosition(position);
	size_t chunkIndex = utils::positionToIndex(chunkPosition, plane->getSize());

	if (chunkMeshesBank.find(chunkIndex) != chunkMeshesBank.end())
	{
		chunkMeshesBank[chunkIndex]->buildGreedy();

		pool.freeChunkMesh(chunkIndex);
		pool.insertChunkMesh(plane, chunkMeshesBank[chunkIndex]);
	}

	glm::uvec3 blockLocalPosition = plane->positionToChunkLocalPosition(position);

	for (unsigned int axis = 0; axis < 3; ++axis)
	{
		glm::uvec3 blockPositionInNeighborChunk = position;

		if (blockLocalPosition[axis] == 0)
		{
			blockPositionInNeighborChunk[axis] -= 1;
		}
		else if (blockLocalPosition[axis] == static_cast<unsigned int>(plane->getChunkSize()) - 1)
		{
			blockPositionInNeighborChunk[axis] += 1;
		}

		if (blockPositionInNeighborChunk != position && plane->isPositionInPlaneBounds(blockPositionInNeighborChunk))
		{
			glm::uvec3 neighborChunkPosition = plane->positionToChunkPosition(blockPositionInNeighborChunk);
			size_t neighborChunkIndex = utils::positionToIndex(neighborChunkPosition, plane->getSize());

			if (chunkMeshesBank.find(neighborChunkIndex) != chunkMeshesBank.end())
			{
				chunkMeshesBank[neighborChunkIndex]->buildGreedy();
				pool.freeChunkMesh(neighborChunkIndex);
				pool.insertChunkMesh(plane, chunkMeshesBank[neighborChunkIndex]);
			}
		}
	}

	cachedPoolData = pool.buildMesh();
}

void LocalClient::setFrustumCullingEnabled(bool enabled)
{
	frustumCullingEnabled = enabled;
}

bool LocalClient::isFrustumCullingEnabled() const
{
	return frustumCullingEnabled;
}

void LocalClient::loadConfig()
{
	std::string configFolderPath = CONFIG_FOLDER + std::string("Players/");
	std::string configFilePath = configFolderPath + name + CONFIG_EXTENSION;
	if (!std::filesystem::exists(configFolderPath))
	{
		std::filesystem::create_directories(configFolderPath);
	}

	if (!std::filesystem::exists(configFilePath))
	{
		std::filesystem::copy_file(CONFIG_FOLDER + std::string("Default.cfg"), configFilePath);
	}

	config.load(configFilePath);
}

void LocalClient::loadMappings()
{
	auto axisEntries = config.getAxisEntries();
	for (const auto& entry : axisEntries)
	{
		if (entry.axis == Axis::UnknownAxis)
		{
			inputHandler->addAxisMapping(entry.key, AxisMapping({ entry.bindingName, entry.scale }));
		}
		else
		{
			inputHandler->addAxisMapping(entry.axis, AxisMapping({ entry.bindingName, entry.scale }));
		}
	}

	auto keyEntries = config.getKeyEntries();
	for (const auto& entry : keyEntries)
	{
		inputHandler->addKeyMapping(entry.key, entry.bindingName);
	}
}

void LocalClient::renderWorld()
{
	renderer->renderMultidraw(cachedPoolData.poolMesh, chunkMaterial, cachedPoolData.sizes.data(), cachedPoolData.offsets.data(), cachedPoolData.drawCount);
}

void LocalClient::renderEntities()
{
	Server* server = app->getCurrentServer();

	if (server)
	{
		for (auto& entityPair : server->getEntities())
		{
			auto& entity = entityPair.second;
			if (auto meshComp = entity->findComponent<MeshComponent>(); meshComp)
			{
				// Don't render your entity as we play in first person
				if (entity->getId() == name)
				{
					// continue;
				}

				renderer->pushRender({ meshComp->getModel()->getMesh(), meshComp->getModel()->getMaterial(), meshComp->getTransform(), viewportId });
			}
		}
	}
}

std::vector<glm::vec3> LocalClient::getSurroundingChunksPositions(const glm::vec3& position, unsigned short viewDistance)
{
	std::vector<glm::vec3> surroundingPositions;

	// Find surrounding chunks
	glm::ivec3 playerChunkPosition = plane->positionToChunkPosition(position);
	for (int i = playerChunkPosition.x - viewDistance; i <= playerChunkPosition.x + viewDistance; ++i)
	{
		for (int j = playerChunkPosition.y - 10; j <= playerChunkPosition.y + 10; ++j)
		{
			for (int k = playerChunkPosition.z - viewDistance; k <= playerChunkPosition.z + viewDistance; ++k)
			{
				// TODO: Find proper way to check validity of chunk position
				if (!(i < 0 || j < 0 || k < 0 || i >= plane->getSize().x || j >= plane->getSize().y || k >= plane->getSize().z))
				{
					surroundingPositions.push_back({ i, j, k });
				}
			}
		}
	}

	std::sort(surroundingPositions.begin(), surroundingPositions.end(), [position](const glm::vec3& pos1, const glm::vec3& pos2) {
		return glm::distance(pos1, position) > glm::distance(pos2, position);
	});

	return surroundingPositions;
}

std::vector<glm::ivec3> LocalClient::cullChunksPositions(const std::vector<glm::ivec3>& chunksPositions)
{
	std::vector<glm::ivec3> culledPositions = chunksPositions;

	

	return culledPositions;
}

void LocalClient::renderDebugMeshes()
{
	for (const DebugMeshInfo& debugMeshInfo : debugInformation->getMeshes())
	{
		renderer->pushRender({ debugMeshInfo.mesh.get(), debugMeshInfo.material, glm::mat4(1.0f), viewportId, debugMeshInfo.renderMode, debugMeshInfo.lineWidth, true, false });
	}
}
