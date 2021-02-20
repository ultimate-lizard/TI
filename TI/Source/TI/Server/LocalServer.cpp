#include "LocalServer.h"

#include <TI/Application.h>
#include <TI/Client/LocalClient.h>
#include <TI/Server/Component/TransformComponent.h>
#include <TI/Server/Component/CameraComponent.h>
#include <TI/Server/Component/MovementComponent.h>
#include <TI/Server/Component/MeshComponent.h>
#include <TI/Renderer/Renderer.h>
#include <TI/Renderer/Camera.h>

LocalServer::LocalServer(Application* app) :
	Server(app)
{
	initEntityTemplates();
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

void LocalServer::disconnectClient(Client* client)
{
	spawnedEntities.erase(client->getName());
}

void LocalServer::createCubes()
{
	// TODO: Spawn() method
	spawnedEntities.emplace("cube", createEntity("Cube", "cube"));
	spawnedEntities.emplace("cube2", createEntity("Cube", "cube2"));

	auto& cubeEntity = spawnedEntities.at("cube");
	auto& cube2Entity = spawnedEntities.at("cube2");

	auto cubeTransform = cubeEntity->findComponent<TransformComponent>();
	if (cubeTransform)
	{
		cubeTransform->setPosition({ 0.0f, 0.0f, -1.0f });
	}

	cubeTransform = cube2Entity->findComponent<TransformComponent>();
	if (cubeTransform)
	{
		cubeTransform->setPosition({ -2.0f, 0.0f, -1.0f });
	}
}

void LocalServer::createPlayerEntity(const std::string& name)
{
	spawnedEntities.emplace(name, createEntity("Player", name));
}

void LocalServer::possesEntity(const std::string& entityName, Client* client)
{
	if (!client)
	{
		return;
	}

	auto& playerEntity = spawnedEntities.at(entityName);
	playerEntity->addComponent<MovementComponent>();

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
