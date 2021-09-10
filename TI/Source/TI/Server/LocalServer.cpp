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

	/*Entity* const cubeEntity = spawnEntity("Cube", "cube");
	Entity* const cube2Entity = spawnEntity("Cube", "cube2");

	if (auto transformComponent = cubeEntity->findComponent<TransformComponent>())
	{
		transformComponent->setPosition({ 0.0f, 0.0f, -1.0f });
	}

	if (auto transformComponent = cube2Entity->findComponent<TransformComponent>())
	{
		transformComponent->setPosition({ -2.0f, 0.0f, -1.0f });
	}*/
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

	if (plane)
	{
		glm::ivec3 planeSize = plane->getSize();

		if (auto transformComponent = player->findComponent<TransformComponent>())
		{
			transformComponent->setPosition({ -1.0f, planeSize.y* plane->getChunkSize() + 2.0f, 0.0f });
		}
	}
	
	possesEntity(client->getName(), client);
}
