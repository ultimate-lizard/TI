#include "Server.h"

#include <TI/Application.h>
#include <TI/Client/Client.h>
#include <TI/Server/Entity.h>
#include <TI/Server/Component/TransformComponent.h>
#include <TI/Server/Component/CameraComponent.h>
#include <TI/Server/Component/MovementComponent.h>
#include <TI/Server/Component/MeshComponent.h>
#include <TI/Renderer/Renderer.h>
#include <TI/Renderer/Camera.h>

static int clientId = 0;

LocalServer::LocalServer(Application* app) :
	Server(app)
{
	createCubes();
}

void LocalServer::update(float dt)
{
	for (auto& entityPair : entities)
	{
		entityPair.second->tick(dt);
	}
}

void LocalServer::connectClient(ClientConnectionRequest request)
{
	// Send packet with client info to IP
	// If IP responds with ConnectionSuccessful packet:
	// client->receiveServerConnectionResponse(ServerConnectionInfo info);
	request.client->receiveServerConnectionResponse({ ++clientId });
	connectedClients.emplace(request.client->getName(), request.client);
	createPlayerEntity(request.client->getName());
}

void LocalServer::createCubes()
{
	entities.emplace("cube", std::make_unique<Entity>());
	entities.emplace("cube2", std::make_unique<Entity>());

	auto& cubeEntity = entities.at("cube");
	auto& cube2Entity = entities.at("cube2");

	// Cube 1
	cubeEntity->addComponent<TransformComponent>(cubeEntity.get(), glm::vec3(0.0f, 0.0f, -1.0f));
	if (app)
	{
		cubeEntity->addComponent<MeshComponent>(cubeEntity.get(), app->getModelManager());
	}
	auto cubeMesh = cubeEntity->findComponent<MeshComponent>();
	if (cubeMesh)
	{
		cubeMesh->loadModel("cube");
	}

	// Cube 2
	cube2Entity->addComponent<TransformComponent>(cube2Entity.get(), glm::vec3(-1.0f, 0.0f, -1.0f));
	if (app)
	{
		cube2Entity->addComponent<MeshComponent>(cube2Entity.get(), app->getModelManager());
	}
	auto cube2Mesh = cube2Entity->findComponent<MeshComponent>();
	if (cube2Mesh)
	{
		cube2Mesh->loadModel("cube");
	}
}

void LocalServer::createPlayerEntity(const std::string& name)
{
	entities.emplace(name, std::make_unique<Entity>());

	auto& playerEntity = entities.at(name);
	playerEntity->addComponent<TransformComponent>(playerEntity.get(), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -90.0f, 0.0f));
	playerEntity->addComponent<MovementComponent>(playerEntity.get());

	if (app)
	{
		playerEntity->addComponent<MeshComponent>(playerEntity.get(), app->getModelManager());
	}
	auto playerMesh = playerEntity->findComponent<MeshComponent>();
	if (playerMesh)
	{
		playerMesh->loadModel("cube");
	}

	possesEntity(name, name);
}

void LocalServer::possesEntity(const std::string& entityName, const std::string& clientName)
{
	auto camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 1.0f));
	camera->setPerspective(
		glm::radians(85.0f),
		static_cast<float>(DEFAULT_WINDOW_WIDTH) / static_cast<float>(DEFAULT_WINDOW_HEIGHT),
		0.01f,
		1000.0f
	);

	auto& playerEntity = entities.at(entityName);

	playerEntity->addComponent<CameraComponent>(playerEntity.get());

	auto cameraComponent = playerEntity->findComponent<CameraComponent>();
	cameraComponent->setCamera(std::move(camera));

	if (app)
	{
		auto renderer = app->getRenderer();
		if (renderer)
		{
			renderer->setCamera(cameraComponent->getCamera());
		}
	}

	auto client = connectedClients.find(clientName);
	if (client != connectedClients.end())
	{
		client->second->possesEntity(playerEntity.get());
	}
}

Entity* const Server::findEntity(const std::string& name)
{
	auto iter = entities.find(name);
	if (iter != entities.end())
	{
		return iter->second.get();
	}

	return nullptr;
}

const std::map<std::string, std::unique_ptr<Entity>>& Server::getEntities() const
{
	return entities;
}
