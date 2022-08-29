#include "LocalServer.h"

#include <TI/Application.h>
#include <TI/Client/LocalClient.h>
#include <TI/Server/Component/CameraComponent.h>
#include <TI/Server/Component/MovementComponent.h>
#include <TI/Server/Component/MeshComponent.h>
#include <TI/Renderer/Renderer.h>
#include <TI/Renderer/Camera.h>
#include <TI/Server/Component/TransformComponent.h>
#include <TI/Server/SceneNode.h>
#include <TI/Server/Plane.h>

LocalServer::LocalServer(Application* app) :
	Server(app)
{
	initEntityTemplates();
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
	if (!planes.empty())
	{
		glm::ivec3 planeSize = planes[0].get()->getSize();

		glm::vec3 spawnLocation { planeSize.x * planes[0].get()->getChunkSize() / 2.0f, planeSize.y * planes[0].get()->getChunkSize() + 3.0f, planeSize.z * planes[0].get()->getChunkSize() / 2.0f};
		Entity* playerEntity = spawnEntity("Player", client->getName(), spawnLocation);
		possesEntity(client->getName(), client);

		spawnEntity("PlanetEntity", "planet_entity", {0.0f, 150.0f, 0.0f});
	}
}
