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
#include <TI/Server/Star.h>
#include <TI/Server/Planet.h>

LocalServer::LocalServer(Application* app) :
	Server(app)
{
	// initHomeSolarSystem();

	//for (size_t i = 0; i < 100; ++i)
	//{
	//	initRandomSolarSystem();
	//}

	// TODO: Remove this
	if (app)
	{
		app->getWindow()->setTitle(GAME_TITLE);
		//app->getWindow()->maximize();
	}

	initEntityTemplates();

	blockGridPtr = std::make_unique<BlockGrid>(glm::uvec3(10), 16);
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
	spawnPlayer(client, nullptr, {});

	return true;
}

void LocalServer::ejectClient(Client* client)
{
	spawnedEntities.erase(client->getName());
}

void LocalServer::spawnPlayer(Client* const client, Planet* planet, const glm::vec3& position)
{
	Entity* playerEntity = spawnEntity("Player", client->getName(), nullptr, position);
	possesEntity(client->getName(), client);

	if (auto transform = playerEntity->findComponent<TransformComponent>())
	{
		float pos = blockGridPtr.get()->getBlockGridSize().x * blockGridPtr.get()->getChunkSize();
		transform->setPosition({ pos / 2.0f, pos + 5.0f, pos / 2.0f });
	}
}
