#include "Server.h"

#include <memory>
#include <stdexcept>

#include <TI/Server/Component/TransformComponent.h>
#include <TI/Server/Component/MeshComponent.h>
#include <TI/Application.h>

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

Entity* const Server::findEntity(const std::string& name)
{
	auto iter = spawnedEntities.find(name);
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
