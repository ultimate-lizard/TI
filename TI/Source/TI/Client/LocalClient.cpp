#include "LocalClient.h"

#include <filesystem>

#include <glm/gtc/matrix_transform.hpp>

#include <TI/Application.h>
#include <TI/Server/LocalServer.h>
#include <TI/Server/BlockGrid.h>
#include <TI/Server/Component/MeshComponent.h>
#include <TI/Server/Component/CameraComponent.h>
#include <TI/Server/Component/TransformComponent.h>
#include <TI/Server/Star.h>
#include <TI/Server/Planet.h>
#include <TI/Client/Input/InputHandler.h>
#include <TI/Client/Controller.h>
#include <TI/Client/AstroBodyMesh.h>
#include <TI/Client/ChunkMesh.h>
#include <TI/Client/DebugInformation.h>
#include <TI/Renderer/Renderer.h>
#include <TI/Renderer/Camera.h>
#include <TI/ResourceManager.h>
#include <TI/Util/Utils.h>
#include <TI/Util/Math.h>

std::unique_ptr<DebugInformation> LocalClient::debugInformation = nullptr;

LocalClient::LocalClient(Application* app, const std::string& name) :
	Client(app),
	viewportId(0),
	renderer(app->getRenderer()),
	chunkMaterial(app->getResourceManager()->getMaterial("Chunk")),
	// plane(nullptr),
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
	meshesBankMutex.lock();
	chunkMeshesBank.clear();
	meshesBankMutex.unlock();
}

void LocalClient::connect(const std::string& ip, int port)
{
	if (Server* server = app->getCurrentServer())
	{
		if (server->connectClient(this, ip, port))
		{
			BlockGrid* plane = server->getStars()[0]->getPlanets()[0]->getBlockGrid();
			activeBlockGrids.push_back(plane);

			drawDebugLine(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(5.0f, 0.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 1.0f);
			drawDebugLine(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 1.0f);
			drawDebugLine(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), 1.0f);

			if (app)
			{
				ResourceManager* rm = app->getResourceManager();

				if (auto localServer = dynamic_cast<LocalServer*>(server))
				{
					initStarSystemVisuals(localServer->getStars()[0].get());
				}

				//if (Model* model = rm->getModel("Star"))
				//{
				//	auto planetMesh = std::make_unique<PlanetMesh>(model);
				//	planetMesh->setPosition({ 0.0f, 0.0f, 0.0f }, CoordinateSystemScale::Interstellar);
				//	planetMesh->setScale(glm::vec3(0.0139f), CoordinateSystemScale::Interstellar);
				//	// planetMesh->rotateInWorldSpace(45.0f, {0.0f, 1.0f, 1.0f});

				//	stars.push_back(std::move(planetMesh));
				//}

				//if (auto localServer = dynamic_cast<LocalServer*>(server))
				//{
				//	// localServer->star
				//	if (Model* model = rm->getModel("Star"))
				//	{
				//		auto planetMesh = std::make_unique<AstroBodyMesh>(localServer->star.get(), model);
				//		planets.push_back(std::move(planetMesh));
				//	}
				//}
			}

			//Model* planetModel = app->getResourceManager()->getModel("Planet");
			//planetMesh = std::make_unique<PlanetMesh>(planetModel);
			//planetMesh->setPosition({ 0.0f, 0.0f, -2.0f });
		}
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
	for (BlockGrid* plane : activeBlockGrids)
	{
		updatePlaneVisuals(plane);
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

void LocalClient::updateBlock(BlockGrid* blockGrid, const glm::uvec3& position)
{
	glm::uvec3 chunkPosition = blockGrid->positionToChunkPosition(position);
	size_t chunkIndex = utils::positionToIndex(chunkPosition, blockGrid->getBlockGridSize());

	if (chunkMeshesBank.find(chunkIndex) != chunkMeshesBank.end())
	{
		chunkMeshesBank[chunkIndex]->buildGreedy();

		pool.freeChunkMesh(chunkIndex);
		pool.insertChunkMesh(blockGrid, chunkMeshesBank[chunkIndex].get());
	}

	glm::uvec3 blockLocalPosition = blockGrid->positionToChunkLocalPosition(position);

	for (unsigned int axis = 0; axis < 3; ++axis)
	{
		glm::uvec3 blockPositionInNeighborChunk = position;

		if (blockLocalPosition[axis] == 0)
		{
			blockPositionInNeighborChunk[axis] -= 1;
		}
		else if (blockLocalPosition[axis] == static_cast<unsigned int>(blockGrid->getChunkSize()) - 1)
		{
			blockPositionInNeighborChunk[axis] += 1;
		}

		if (blockPositionInNeighborChunk != position && blockGrid->isPositionInGridBounds(blockPositionInNeighborChunk))
		{
			glm::uvec3 neighborChunkPosition = blockGrid->positionToChunkPosition(blockPositionInNeighborChunk);
			size_t neighborChunkIndex = utils::positionToIndex(neighborChunkPosition, blockGrid->getBlockGridSize());

			if (chunkMeshesBank.find(neighborChunkIndex) != chunkMeshesBank.end())
			{
				chunkMeshesBank[neighborChunkIndex]->buildGreedy();
				pool.freeChunkMesh(neighborChunkIndex);
				pool.insertChunkMesh(blockGrid, chunkMeshesBank[neighborChunkIndex].get());
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

void LocalClient::initStarSystemVisuals(Star* star)
{
	if (Server* server = app->getCurrentServer())
	{
		if (auto localServer = dynamic_cast<LocalServer*>(server))
		{
			ResourceManager* rm = app->getResourceManager();
			if (Model* model = rm->getModel("Star"))
			{
				auto planetMesh = std::make_unique<AstroBodyMesh>(star, model);
				planets.push_back(std::move(planetMesh));
			}

			for (const std::unique_ptr<Planet>& orbitingBody : star->getPlanets())
			{
				if (Model* model = rm->getModel("Planet"))
				{
					auto planetMesh = std::make_unique<AstroBodyMesh>(orbitingBody.get(), model);
					planets.push_back(std::move(planetMesh));
				}
			}
		}
	}
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

void LocalClient::updatePlaneVisuals(BlockGrid* blockGrid)
{
	if (possessedEntity && blockGrid)
	{
		if (auto transformComponent = possessedEntity->findComponent<TransformComponent>())
		{
			if (chunkMaterial)
			{
				if (Shader* shader = chunkMaterial->getShader())
				{
					shader->setVector3("lightPos", transformComponent->getPosition());
				}
			}

			// TODO: Find a way to get the chunk an entity stands on
			glm::vec3 playerPosition = transformComponent->getPosition();
			playerPosition.y -= 3.0f;

			if (playerLastChunkPosition != blockGrid->positionToChunkPosition(playerPosition))
			{
				playerLastChunkPosition = blockGrid->positionToChunkPosition(playerPosition);

				// TODO: Pass the chunk pool a list of visible chunks?
				for (const auto& position : cachedVisibleChunksPositions)
				{
					pool.setChunkMeshVisibility(utils::positionToIndex(position, blockGrid->getBlockGridSize()), false);
				}

				const int VIEW_DISTANCE = 32;
				cachedVisibleChunksPositions = getSurroundingChunksPositions(blockGrid, playerPosition, VIEW_DISTANCE);

				std::cout << "EBO size: " << pool.analyticEboSize / 1024 / 1024 << " MB" << std::endl;
				std::cout << "VBO size: " << pool.analyticVboSize / 1024 / 1024 << " MB" << std::endl;
			}

			if (cachedEntityCamera)
			{
				Frustum frustum(cachedEntityCamera->getProjection() * cachedEntityCamera->getView());

				for (size_t i = 0; i < cachedVisibleChunksPositions.size(); ++i)
				{
					const glm::ivec3& chunkPosition = cachedVisibleChunksPositions[i];
					const size_t chunkSize = blockGrid->getChunkSize();

					glm::ivec3 absoluteChunkPosition(chunkPosition.x * chunkSize, chunkPosition.y * chunkSize, chunkPosition.z * chunkSize);
					if (frustum.IsBoxVisible(absoluteChunkPosition, { absoluteChunkPosition.x + chunkSize, absoluteChunkPosition.y + chunkSize, absoluteChunkPosition.z + chunkSize }))
					{
						const size_t chunkIndex = utils::positionToIndex(chunkPosition, blockGrid->getBlockGridSize());
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
								pool.insertChunkMesh(blockGrid, foundIter->second.get());
							}
						}
						// If mesh is not yet built
						else
						{
							// If not pending, create task
							if (std::find(pendingChunks.begin(), pendingChunks.end(), chunkIndex) == pendingChunks.end())
							{
								{
									// std::unique_lock<std::mutex> lock(meshesBankMutex);
									pendingChunks.push_back(chunkIndex);
								}

								app->threadPool.pushTask([this, blockGrid, chunkPosition, chunkIndex]() {
									{
										ChunkMesh* newMesh = new ChunkMesh(blockGrid->getChunk(chunkPosition), blockGrid);
										std::unique_lock<std::mutex> lock(meshesBankMutex);
										chunkMeshesBank.emplace(chunkIndex, newMesh);
										auto foundIter = std::find(pendingChunks.begin(), pendingChunks.end(), chunkIndex);
										pendingChunks.erase(foundIter);
									}
									});
							}
							meshesBankMutex.unlock();
						}
					}
					else
					{
						pool.setChunkMeshVisibility(utils::positionToIndex(chunkPosition, blockGrid->getBlockGridSize()), false);
					}
				}

				cachedPoolData = pool.buildMesh();
			}
		}
	}
}

void LocalClient::renderWorld()
{
	//float dist = 0.0f;
	//if (const Server* server = app->getCurrentServer())
	//{
	//	if (auto result = server->getEntities().find("planet_entity"); result != server->getEntities().end())
	//	{
	//		if (auto mesh = result->second.get()->findComponent<MeshComponent>())
	//		{
	//			if (Model* model = mesh->getModel())
	//			{
	//				if (Material* mat = model->getMaterial())
	//				{
	//					if (Shader* shader = mat->getShader())
	//					{
	//						if (possessedEntity)
	//						{
	//							if (auto trans = possessedEntity->findComponent<TransformComponent>())
	//							{
	//								if (plane)
	//								{
	//									glm::vec3 center = { plane->getSize().x * plane->getChunkSize() / 2.0f, plane->getSize().y * plane->getChunkSize() / 2.0f, plane->getSize().z * plane->getChunkSize() / 2.0f };
	//									glm::vec3 playerPosition = trans->getPosition();
	//									dist = glm::distance(center, playerPosition);
	//									shader->setFloat("playerDistance", dist);
	//								}
	//							}
	//						}
	//					}
	//				}
	//			}
	//		}
	//	}
	//}

	//chunkMaterial->getShader()->use();
	//chunkMaterial->getShader()->setFloat("playerDistanceChunk", dist);

	RenderCommand cmd;
	cmd.mesh = cachedPoolData.poolMesh;
	cmd.material = chunkMaterial;
	cmd.transform = glm::mat4(1.0f);
	cmd.viewportId = getViewportId();
	cmd.counts = cachedPoolData.sizes.data();
	cmd.indices = cachedPoolData.offsets.data();
	cmd.multiDrawCount = cachedPoolData.drawCount;
	renderer->pushRender(cmd, 0);
	
	//for (auto& planetMesh : stars)
	//{
	//	if (planetMesh)
	//	{
	//		RenderCommand cmd2;
	//		cmd2.mesh = planetMesh->getModel()->getMesh();
	//		cmd2.material = planetMesh->getModel()->getMaterial();
	//		cmd2.transform = planetMesh->getAstroBody()->getTransform(CoordinateSystemScale::Interstellar);
	//		cmd2.viewportId = getViewportId();
	//		cmd2.cullFaces = false;
	//		renderer->pushRender(cmd2, 0);
	//	}
	//}
	
	for (auto& planetMesh : planets)
	{
		if (planetMesh)
		{
			RenderCommand cmd2;
			cmd2.mesh = planetMesh->getModel()->getMesh();
			cmd2.material = planetMesh->getModel()->getMaterial();
			cmd2.transform = planetMesh->getAstroBody()->getTransform(CoordinateSystem::Interplanetary);
			cmd2.viewportId = getViewportId();
			cmd2.cullFaces = false;
			renderer->pushRender(cmd2, 1);
		}
	}
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

				if (auto transformComponent = entity->findComponent<TransformComponent>())
				{
					renderer->pushRender({ meshComp->getModel()->getMesh(), meshComp->getModel()->getMaterial(), meshComp->getTransform(), viewportId }, 0);
				}
			}
		}
	}
}

std::vector<glm::vec3> LocalClient::getSurroundingChunksPositions(BlockGrid* blockGrid, const glm::vec3& position, unsigned short viewDistance)
{
	std::vector<glm::vec3> surroundingPositions;

	// Find surrounding chunks
	glm::ivec3 playerChunkPosition = blockGrid->positionToChunkPosition(position);
	for (int i = playerChunkPosition.x - viewDistance; i <= playerChunkPosition.x + viewDistance; ++i)
	{
		for (int j = playerChunkPosition.y - 10; j <= playerChunkPosition.y + 10; ++j)
		{
			for (int k = playerChunkPosition.z - viewDistance; k <= playerChunkPosition.z + viewDistance; ++k)
			{
				// TODO: Find proper way to check validity of chunk position
				if (!(i < 0 || j < 0 || k < 0 || i >= blockGrid->getBlockGridSize().x || j >= blockGrid->getBlockGridSize().y || k >= blockGrid->getBlockGridSize().z))
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
		renderer->pushRender({ debugMeshInfo.mesh.get(), debugMeshInfo.material, glm::mat4(1.0f), viewportId, debugMeshInfo.renderMode, debugMeshInfo.lineWidth, true, false }, 0);
	}
}
