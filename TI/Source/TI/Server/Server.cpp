#include "Server.h"

#include <memory>
#include <stdexcept>

#include <TI/Server/Component/TransformComponent.h>
#include <TI/Server/Component/MeshComponent.h>
#include <TI/Server/Component/MovementComponent.h>
#include <TI/Server/Component/CameraComponent.h>
#include <TI/Application.h>
#include <TI/Client/LocalClient.h>
#include <TI/Renderer/Camera.h>

void Server::initEntityTemplates()
{
	// Player
	auto player = std::make_unique<Entity>();
	player->setName("Player");

	auto transformComp = player->addComponent<TransformComponent>(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -90.0f, 0.0f));
	transformComp->setScale({ 0.3f, 0.3f, 0.3f });

	auto meshComp = player->addComponent<MeshComponent>(app->getModelManager());
	meshComp->loadModel("player");

	entityTemplates.emplace(player->getName(), std::move(player));

	// Cube
	auto cube = std::make_unique<Entity>();
	cube->setName("Cube");

	cube->addComponent<TransformComponent>();
	auto cubeMesh = cube->addComponent<MeshComponent>(app->getModelManager());
	cubeMesh->loadModel("cube");

	entityTemplates.emplace(cube->getName(), std::move(cube));
}

void Server::update(float dt)
{
	if (shuttingDown)
	{
		shutdown();
	}
}

void Server::requestShutdown()
{
	shuttingDown = true;
}

void Server::shutdown()
{
	app->requestQuit();
}

Entity* const Server::findEntity(const std::string& id)
{
	
	if (auto iter = spawnedEntities.find(id); iter != spawnedEntities.end())
	{
		return iter->second.get();
	}

	return nullptr;
}

void Server::removeEntity(const std::string& id)
{
	if (auto iter = spawnedEntities.find(id); iter != spawnedEntities.end())
	{
		spawnedEntities.erase(iter);
	}
}

const std::map<std::string, std::unique_ptr<Entity>>& Server::getEntities() const
{
	return spawnedEntities;
}

std::unique_ptr<Entity> Server::createEntity(const std::string& name, const std::string& id)
{
	if (auto iter = entityTemplates.find(name); iter != entityTemplates.end())
	{
		auto newEntity = iter->second->clone();
		newEntity->setId(id);

		return std::move(newEntity);
	}
	
	throw std::exception();
}

void Server::spawnPlayerEntity(const std::string& name)
{
	if (!findEntity(name))
	{
		spawnedEntities.emplace(name, createEntity("Player", name));
	}
}

void Server::possesEntity(const std::string& entityName, Client* client)
{
	if (!client)
	{
		return;
	}

	if (spawnedEntities.find(entityName) != spawnedEntities.end())
	{
		std::unique_ptr<Entity>& possesedEntity = spawnedEntities.at(entityName);
		possesedEntity->addComponent<MovementComponent>();

		if (Renderer* renderer = app->getRenderer())
		{
			// If the client is a human, add a camera to its entity
			auto localClient = dynamic_cast<LocalClient*>(client);
			if (localClient)
			{
				auto camera = std::make_unique<Camera>();

				possesedEntity->addComponent<CameraComponent>();

				auto cameraComponent = possesedEntity->findComponent<CameraComponent>();
				cameraComponent->setCamera(std::move(camera));
			}
		}

		client->possesEntity(possesedEntity.get());
	}
}

void Server::spawnPlayer(Client* client)
{

}
