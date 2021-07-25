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
	createEntityTemplates();
	createCubes();
}

void LocalServer::update(float dt)
{
	Server::update(dt);

	for (auto& entityPair : spawnedEntities)
	{
		entityPair.second->tick(dt);
	}
}

void LocalServer::connectClient(Client* client, const std::string& ip, int port)
{
	spawnEntity(client->getName(), "Player");
	possesEntity(client->getName(), client);
}

void LocalServer::ejectClient(Client* client)
{
	spawnedEntities.erase(client->getName());
}

void LocalServer::createCubes()
{
	// TODO: Spawn() method
	spawnEntity("Cube", "cube");
	spawnEntity("Cube", "cube2");

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

