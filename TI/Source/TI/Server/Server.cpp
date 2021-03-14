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
	auto player = std::make_unique<Entity>();
	player->setName("Player");

	player->addComponent<TransformComponent>(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -90.0f, 0.0f));
	auto transformComp = player->findComponent<TransformComponent>();
	transformComp->setScale({ 0.3f, 0.3f, 0.3f });

	if (app)
	{
		player->addComponent<MeshComponent>(app->getModelManager());
		auto meshComp = player->findComponent<MeshComponent>();
		meshComp->loadModel("cube");
	}

	entityTemplates.emplace(player->getName(), std::move(player));

	auto cube = std::make_unique<Entity>();
	cube->setName("Cube");

	cube->addComponent<TransformComponent>();
	if (app)
	{
		cube->addComponent<MeshComponent>(app->getModelManager());
	}

	auto cubeMesh = cube->findComponent<MeshComponent>();
	if (cubeMesh)
	{
		cubeMesh->loadModel("cube");
	}

	entityTemplates.emplace(cube->getName(), std::move(cube));
}

int Server::clientId = 0;

void Server::shutdown()
{
	if (app)
	{
		app->requestQuit();
	}
}

Entity* const Server::findEntity(const std::string& id)
{
	auto iter = spawnedEntities.find(id);
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

void Server::createPlayerEntity(const std::string& name)
{
	spawnedEntities.emplace(name, createEntity("Player", name));
}

void Server::possesEntity(const std::string& entityName, Client* client)
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
