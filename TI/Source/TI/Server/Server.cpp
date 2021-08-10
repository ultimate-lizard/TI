#include "Server.h"

#include <memory>
#include <stdexcept>

#include <TI/Server/Component/MeshComponent.h>
#include <TI/Server/Component/MovementComponent.h>
#include <TI/Server/Component/CameraComponent.h>
#include <TI/Server/Component/TransformComponent.h>
#include <TI/Application.h>
#include <TI/Client/LocalClient.h>
#include <TI/Renderer/Camera.h>

void Server::createEntityTemplates()
{
	// Player ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	auto playerEntity = std::make_unique<Entity>();
	playerEntity->addComponent<TransformComponent>();

	auto camera = std::make_unique<Camera>();
	playerEntity->setName("Player");

	if (auto transformComponent = playerEntity->findComponent<TransformComponent>())
	{
		transformComponent->setScale({ 0.3f, 0.3f, 0.3f });
	}

	auto meshComponent = playerEntity->addComponent<MeshComponent>(app->getModelManager());
	meshComponent->loadModel("player");

	// Setup local coordinates for the camera
	camera->setRotation({ 0.0f, 90.0f, 0.0f });
	camera->setPosition({ -1.2f, 1.0f, -0.0f });
	// camera->setParentNode(playerEntity.get());

	auto cameraComponent = playerEntity->addComponent<CameraComponent>();
	cameraComponent->setCamera(std::move(camera));

	entityTemplates.emplace(playerEntity->getName(), std::move(playerEntity));

	// Cube ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	auto cubeEntity = std::make_unique<Entity>();
	cubeEntity->addComponent<TransformComponent>();

	cubeEntity->setName("Cube");

	auto cubeMeshComponent = cubeEntity->addComponent<MeshComponent>(app->getModelManager());
	cubeMeshComponent->loadModel("cube");
	
	entityTemplates.emplace(cubeEntity->getName(), std::move(cubeEntity));
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

std::unique_ptr<Entity> Server::createEntityFromTemplate(const std::string& name, const std::string& id)
{
	if (auto iter = entityTemplates.find(name); iter != entityTemplates.end())
	{
		auto newEntity = iter->second->clone();
		newEntity->setId(id);
		
		return std::move(newEntity);
	}
	
	throw std::exception();
}

Entity* const Server::spawnEntity(const std::string& templateName, const std::string& id)
{
	Entity* result = nullptr;

	if (!findEntity(id))
	{
		std::unique_ptr<Entity> spawnedEntity = createEntityFromTemplate(templateName, id);
		result = spawnedEntity.get();
		spawnedEntities.emplace(id, std::move(spawnedEntity));
	}

	return result;
}

void Server::possesEntity(const std::string& entityName, Client* const client)
{
	if (!client)
	{
		return;
	}

	if (spawnedEntities.find(entityName) != spawnedEntities.end())
	{
		std::unique_ptr<Entity>& possesedEntity = spawnedEntities.at(entityName);

		if (auto transformComponent = possesedEntity->findComponent<TransformComponent>())
		{
			possesedEntity->addComponent<MovementComponent>(transformComponent);
		}
		else
		{
			throw std::exception("Cannot posses entity without transform component");
		}
		
		client->possesEntity(possesedEntity.get());
	}
}
