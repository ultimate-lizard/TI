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

std::unique_ptr<DebugInformation> LocalClient::debugInformation = nullptr;

LocalClient::LocalClient(Application* app, const std::string& name) :
	Client(app),
	viewportId(0),
	renderer(app->getRenderer()),
	chunkMaterial(app->getResourceManager()->getMaterial("Chunk")),
	plane(nullptr)
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
	for (auto& mesh : visibleChunkMeshes)
	{
		if (mesh.second)
		{
			delete mesh.second;
		}
	}

	visibleChunkMeshes.clear();
}

void LocalClient::connect(const std::string& ip, int port)
{
	if (Server* server = app->getCurrentServer())
	{
		server->connectClient(this, ip, port);

		//const std::vector<Chunk>& chunks = server->getPlane()->getChunks();
		//for (unsigned int i = 0; i < chunks.size(); ++i)
		//{
		//	// visibleChunkMeshes.push_back(new ChunkMesh(&chunks[i], server->getPlane()));
		//	visibleChunkMeshes.emplace(utils::positionToIndex(chunks.at(i).getPosition(), plane->getSize()), new ChunkMesh(&chunks[i], server->getPlane()));
		//}

		plane = server->getPlane();

		for (const Chunk& chunk: plane->getChunks())
		{
			visibleChunkMeshes.emplace(utils::positionToIndex(plane->positionToChunkPosition(chunk.getPosition()), plane->getSize()), new ChunkMesh(&chunk,  plane));
		}

		drawDebugLine(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 1.0f);
		drawDebugLine(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 1.0f);
		drawDebugLine(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), 1.0f);

		rebuildPlaneMesh();
	}
}

void LocalClient::setPossesedEntity(Entity* entity)
{
	Client::setPossesedEntity(entity);

	playerController->posses(entity);

	if (!entity)
	{
		return;
	}

	auto cameraComp = entity->findComponent<CameraComponent>();
	if (!cameraComp)
	{
		return;
	}

	auto renderer = app->getRenderer();
	if (!renderer)
	{
		return;
	}

	auto viewport = renderer->getViewport(viewportId);
	if (viewport)
	{
		viewport->setActiveCamera(cameraComp->getCamera());
	}
}

void LocalClient::update(float dt)
{
	/*if (possessedEntity)
	{
		if (const auto transformComponent = possessedEntity->findComponent<TransformComponent>())
		{
			if (Plane* currentPlane = transformComponent->getPlane())
			{
				const int VIEW_DISTANCE = 16;
				const size_t CHUNK_SIZE = plane->getChunkSize();

				glm::uvec3 entityPosition = transformComponent->getPosition();

				std::vector<size_t> visibleChunksIndices;

				if (playerLastChunkLocation != plane->positionToChunkPosition(entityPosition))
				{
					for (int x = -VIEW_DISTANCE; x <= VIEW_DISTANCE; ++x)
					{
						for (int y = -10; y <= 10; ++y)
						{
							for (int z = -VIEW_DISTANCE; z <= VIEW_DISTANCE; ++z)
							{
								glm::uvec3 visibleChunkAbsolutePosition(entityPosition.x + x * CHUNK_SIZE, entityPosition.y + y * CHUNK_SIZE, entityPosition.z + z * CHUNK_SIZE);
								glm::uvec3 visibleChunkPosition = plane->positionToChunkPosition(visibleChunkAbsolutePosition);
								size_t chunkIndex = utils::positionToIndex(visibleChunkPosition, plane->getSize());

								if (const Chunk* const serverChunk = currentPlane->getChunk(visibleChunkPosition))
								{
									visibleChunksIndices.push_back(chunkIndex);

									if (visibleChunkMeshes.find(chunkIndex) == visibleChunkMeshes.end())
									{
										if (cachedChunkMeshes.find(chunkIndex) != cachedChunkMeshes.end())
										{
											visibleChunkMeshes.insert(cachedChunkMeshes.extract(chunkIndex));
										}
										else
										{
											visibleChunkMeshes.emplace(chunkIndex, new ChunkMesh(serverChunk, currentPlane));
										}
									}
								}

							}
						}
					}

					for (auto visibleChunkMeshPair : visibleChunkMeshes)
					{
						if (std::find(visibleChunksIndices.begin(), visibleChunksIndices.end(), visibleChunkMeshPair.first) == visibleChunksIndices.end())
						{
							cachedChunkMeshes.insert(visibleChunkMeshes.extract(visibleChunkMeshPair.first));
						}
					}
				}
			}
		}
	}*/

	// TODO: assert(renderer)
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

	if (visibleChunkMeshes.find(chunkIndex) != visibleChunkMeshes.end())
	{
		// visibleChunkMeshes[chunkIndex]->updateBlock(plane->positionToChunkLocalPosition(position));
		visibleChunkMeshes[chunkIndex]->buildGreedy();
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

			if (visibleChunkMeshes.find(neighborChunkIndex) != visibleChunkMeshes.end())
			{
				// visibleChunkMeshes[neighborChunkIndex]->updateBlock(plane->positionToChunkLocalPosition(blockPositionInNeighborChunk));
				visibleChunkMeshes[neighborChunkIndex]->buildGreedy();
			}
		}
	}

	rebuildPlaneMesh();
}

void LocalClient::rebuildPlaneMesh()
{
	indicesCount.clear();
	indices.clear();

	if (!planeMesh)
	{
		planeMesh = std::make_unique<Mesh>();
	}

	if (!planeMesh->isDynamic())
	{
		MeshBuilder builder;
		planeMesh = builder.buildDyanmic(0, 0);
	}


	size_t vboSize = 0;
	size_t eboSize = 0;

	std::vector<float> verticesData;
	std::vector<unsigned int> indicesData;

	// std::vector<size_t> eboOffsets

	// size_t i = 0;
	size_t i = 0;
	// indicesData.resize(visibleChunkMeshes.)

	int lastSize = 0;
	int offset = 0;
	for (auto& mapPair : visibleChunkMeshes)
	{
		// indices[i] = new unsigned int[mapPair.second->elements.size()];
		// indices.push_back(reinterpret_cast<void*>(mapPair.second));

		//for (unsigned int j = 0; j < mapPair.second->elements.size(); ++j)
		//{
		//	// indices[i][j] = mapPair.second->elements[j];
		//	indices.push_back(reinterpret_cast<void*>(mapPair.second->elements[j]));
		//}

		indices.push_back(reinterpret_cast<void*>(offset));
		offset += mapPair.second->elements.size() * sizeof(unsigned int);

		// eboOffset += mapPair.second->elements.size();

		vboSize += mapPair.second->data.size() * sizeof(float);
		eboSize += mapPair.second->elements.size() * sizeof(unsigned int);

		verticesData.insert(verticesData.end(), mapPair.second->data.begin(), mapPair.second->data.end());
		// indicesData.insert(indicesData.end(), mapPair.second->elements.begin(), mapPair.second->elements.end());
		for (size_t i = 0; i < mapPair.second->elements.size(); ++i)
		{
			indicesData.push_back(mapPair.second->elements[i] + lastSize / 6 * 4);
		}


		lastSize += mapPair.second->elements.size();
		indicesCount.push_back(mapPair.second->elements.size());

		++i;
	}

	planeMesh->setBufferSize(vboSize);
	planeMesh->setBufferSubData(0, verticesData);
	planeMesh->setPositionsCount(verticesData.size());

	planeMesh->setElementsSize(eboSize);
	planeMesh->setElementsSubData(0, indicesData);
	planeMesh->setIndicesCount(indicesData.size());
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
	renderer->pushRender({ planeMesh.get(), chunkMaterial, glm::mat4(1.0f), viewportId });
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

void LocalClient::renderDebugMeshes()
{
	for (const DebugMeshInfo& debugMeshInfo : debugInformation->getMeshes())
	{
		renderer->pushRender({ debugMeshInfo.mesh.get(), debugMeshInfo.material, glm::mat4(1.0f), viewportId, debugMeshInfo.renderMode, debugMeshInfo.lineWidth, true, false });
	}
}
