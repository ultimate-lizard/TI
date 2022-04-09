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
	if (plane)
	{
		glm::ivec3 planeSize = plane->getSize();

		glm::vec3 spawnLocation { planeSize.x * plane->getChunkSize() / 2.0f, planeSize.y * plane->getChunkSize() + 3.0f, planeSize.z * plane->getChunkSize() / 2.0f };
		Entity* playerEntity = spawnEntity("Player", client->getName(), spawnLocation);
		possesEntity(client->getName(), client);
		auto cameraComp = playerEntity->findComponent<CameraComponent>();
		Camera* cam = cameraComp->getCamera();
		cameraComp->setParent(playerEntity->findComponent<TransformComponent>());
		cam->setParent(cameraComp);
		// cam->setParent(playerEntity->findComponent<TransformComponent>());
		//spawnLocation.x += 5.0f;

		// spawnEntity("Cube", "Cube1", spawnLocation);

	}
}
