#include "LocalServer.h"

#include <TI/Application.h>
#include <TI/Client/LocalClient.h>
#include <TI/Server/Component/CameraComponent.h>
#include <TI/Server/Component/MovementComponent.h>
#include <TI/Server/Component/MeshComponent.h>
#include <TI/Renderer/Renderer.h>
#include <TI/Renderer/Camera.h>

LocalServer::LocalServer(Application* app) :
	Server(app)
{
	createEntityTemplates();

	Entity* const cubeEntity = spawnEntity("Cube", "cube");
	Entity* const cube2Entity = spawnEntity("Cube", "cube2");

	cubeEntity->setPosition({ 0.0f, 0.0f, -1.0f });
	cube2Entity->setPosition({ -2.0f, 0.0f, -1.0f });
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
	spawnPlayer(client);
}

void LocalServer::ejectClient(Client* client)
{
	spawnedEntities.erase(client->getName());
}

void LocalServer::spawnPlayer(Client* const client)
{
	Entity* const player = spawnEntity(client->getName(), "Player");
	player->setPosition({ 0.0f, 0.0f, 0.0f });
	// player->findComponent<CameraComponent>()->getCamera()->setParent(player);
	possesEntity(client->getName(), client);
}
