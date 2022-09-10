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
#include <TI/Server/BlockGrid.h>
// #include <TI/Server/AstroBody.h>
#include <TI/Server/Star.h>
#include <TI/Server/Planet.h>

LocalServer::LocalServer(Application* app) :
	Server(app)
{
	initStarSystems();

	// TODO: Remove this
	if (app)
	{
		app->getWindow()->setTitle(GAME_TITLE);
		//app->getWindow()->maximize();
	}

	initEntityTemplates();
}

LocalServer::~LocalServer()
{
}

void LocalServer::update(float dt)
{
	Server::update(dt);

	for (auto& entityPair : spawnedEntities)
	{
		entityPair.second->tick(dt);
	}
}

bool LocalServer::connectClient(Client* client, const std::string& ip, int port)
{
	// Get home planet for spawn
	if (BlockGrid* plane = stars[0]->getPlanets()[0]->getPlane())
	{
		glm::ivec3 planeSize = plane->getBlockGridSize();

		glm::vec3 spawnLocation{ planeSize.x * plane->getChunkSize() / 2.0f, planeSize.y * plane->getChunkSize() + 3.0f, planeSize.z * plane->getChunkSize() / 2.0f };

		spawnPlayer(client, plane, spawnLocation);
	}

	return true;
}

void LocalServer::ejectClient(Client* client)
{
	spawnedEntities.erase(client->getName());
}

void LocalServer::initStarSystems()
{
	// planes.push_back(std::make_unique<Plane>(glm::vec3(0.0f), glm::uvec3(10), 16));
	stars.push_back(std::make_unique<Star>());
}

void LocalServer::spawnPlayer(Client* const client, BlockGrid* plane, const glm::vec3& position)
{
	if (plane)
	{
		// glm::ivec3 planeSize = plane->getSize();

		// glm::vec3 spawnLocation { planeSize.x * planes[0].get()->getChunkSize() / 2.0f, planeSize.y * planes[0].get()->getChunkSize() + 3.0f, planeSize.z * planes[0].get()->getChunkSize() / 2.0f};
		Entity* playerEntity = spawnEntity("Player", client->getName(), plane, position);
		possesEntity(client->getName(), client);
	}
}
