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
	for (auto& entityPair : spawnedEntities)
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
	possesEntity(request.client->getName(), request.client);
}

void LocalServer::initEntityTemplates()
{
	
}

void LocalServer::createCubes()
{
	spawnedEntities.emplace("cube", std::make_unique<Entity>());
	spawnedEntities.emplace("cube2", std::make_unique<Entity>());

	auto& cubeEntity = spawnedEntities.at("cube");
	auto& cube2Entity = spawnedEntities.at("cube2");

	// Cube 1
	cubeEntity->addComponent<TransformComponent>(glm::vec3(0.0f, 0.0f, -1.0f));
	if (app)
	{
		cubeEntity->addComponent<MeshComponent>(app->getModelManager());
	}
	auto cubeMesh = cubeEntity->findComponent<MeshComponent>();
	if (cubeMesh)
	{
		cubeMesh->loadModel("cube");
	}

	// Cube 2
	cube2Entity->addComponent<TransformComponent>(glm::vec3(-2.0f, 0.0f, -1.0f));
	if (app)
	{
		cube2Entity->addComponent<MeshComponent>(app->getModelManager());
	}
	auto cube2Mesh = cube2Entity->findComponent<MeshComponent>();
	if (cube2Mesh)
	{
		cube2Mesh->loadModel("cube");
	}
}

void LocalServer::createPlayerEntity(const std::string& name)
{
	spawnedEntities.emplace(name, std::make_unique<Entity>());

	auto& playerEntity = spawnedEntities.at(name);
	playerEntity->setName(name);
	playerEntity->addComponent<MovementComponent>();

	playerEntity->addComponent<TransformComponent>(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -90.0f, 0.0f));
	auto transformComp = playerEntity->findComponent<TransformComponent>();
	transformComp->setScale({ 0.3f, 0.3f, 0.3f });

	playerEntity->addComponent<MeshComponent>(app->getModelManager());
	auto meshComp = playerEntity->findComponent<MeshComponent>();
	meshComp->loadModel("cube");
}

void LocalServer::possesEntity(const std::string& entityName, Client* client)
{
	if (!client)
	{
		return;
	}

	auto& playerEntity = spawnedEntities.at(entityName);

	if (app)
	{
		auto renderer = app->getRenderer();
		if (renderer)
		{
			// If the client is a human, add a camera to its entity
			auto localClient = dynamic_cast<LocalClient*>(client);
			if (localClient)
			{
				auto camera = std::make_unique<Camera>();

				playerEntity->addComponent<CameraComponent>();

				auto cameraComponent = playerEntity->findComponent<CameraComponent>();
				cameraComponent->setCamera(std::move(camera));
			}
		}
	}

	client->possesEntity(playerEntity.get());
}

Entity* const Server::findEntity(const std::string& name)
{
	auto iter = spawnedEntities.find(name);
	if (iter != spawnedEntities.end())
	{
		return iter->second.get();
	}

	return nullptr;
}

const std::map<std::string, std::unique_ptr<Entity>>& Server::getEntities() const
{
	return spawnedEntities;
}
