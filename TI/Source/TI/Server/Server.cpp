#include "Server.h"

#include <stdexcept>

#include <TI/Server/Component/MeshComponent.h>
#include <TI/Server/Component/MovementComponent.h>
#include <TI/Server/Component/CameraComponent.h>
#include <TI/Server/Component/TransformComponent.h>
#include <TI/Server/Component/CollisionComponent.h>
#include <TI/Server/Component/PhysicsComponent.h>
#include <TI/Application.h>
#include <TI/Client/LocalClient.h>
#include <TI/Renderer/Camera.h>
#include <TI/Server/BlockGrid.h>
#include <TI/Server/Star.h>
#include <TI/Server/SolarSystem.h>

void Server::initEntityTemplates()
{
	auto playerEntity = std::make_unique<Entity>();
	playerEntity->setName("Player");

	auto transformComponent = playerEntity->addComponent<TransformComponent>();

	auto meshComponent = playerEntity->addComponent<MeshComponent>(app->getResourceManager());
	meshComponent->loadModel("Player");
	meshComponent->setLocalScale({ 0.6f, 0.6f, 1.0f });
	meshComponent->setVisibleForParentEntity(true);

	auto collisionComponent = playerEntity->addComponent<CollisionComponent>();
	collisionComponent->setCollisionBox({ { 0.6f, 1.9f, 0.6f }, { 0.0f, 0.85f, 0.0f } });

	auto movementComponent = playerEntity->addComponent<MovementComponent>();

	auto cameraComponent = playerEntity->addComponent<CameraComponent>();
	cameraComponent->setParent(transformComponent);
	cameraComponent->setLocalPosition({ 0.0f, 1.75f, 0.0f });

	meshComponent->setParent(cameraComponent);

	entityTemplates.emplace(playerEntity->getName(), std::move(playerEntity));

	auto cubeEntity = std::make_unique<Entity>();
	cubeEntity->setName("Cube");
	auto cubeTransformComponent = cubeEntity->addComponent<TransformComponent>();
	auto cubeMeshComponent = cubeEntity->addComponent<MeshComponent>(app->getResourceManager());
	cubeMeshComponent->setParent(cubeTransformComponent);
	cubeMeshComponent->loadModel("Cube");
	// cubeMeshComponent->setLocalScale({ 1.0f, 2.0f, 1.0f });
	auto cubeCollisionComponent = cubeEntity->addComponent<CollisionComponent>();
	// cubeCollisionComponent->setCollisionBox({ {1.0f, 2.0f, 1.0f}, glm::vec3(0.0f) });
	cubeCollisionComponent->setCollisionBox({ {1.0f, 1.0f, 1.0f}, glm::vec3(0.0f) });
	auto cubePhysicsComponent = cubeEntity->addComponent<PhysicsComponent>();

	entityTemplates.emplace(cubeEntity->getName(), std::move(cubeEntity));

	auto planetEntity = std::make_unique<Entity>();
	auto planetTransformComponent = planetEntity->addComponent<TransformComponent>();
	auto planetMeshComponent = planetEntity->addComponent<MeshComponent>(app->getResourceManager());

	planetEntity->setName("PlanetEntity");

	planetTransformComponent->setLocalScale(glm::vec3(55.0f));
	planetMeshComponent->setParent(planetTransformComponent);
	planetMeshComponent->loadModel("Planet");

	entityTemplates.emplace(planetEntity->getName(), std::move(planetEntity));

}

Server::Server(Application* app) :
	app(app),
	shuttingDown(false)
{
}

Server::~Server()
{
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
	
	return nullptr;
}

Entity* const Server::spawnEntity(const std::string& templateName, const std::string& id, BlockGrid* blockGrid, const glm::vec3& position)
{
	Entity* result = nullptr;

	if (!findEntity(id))
	{
		std::unique_ptr<Entity> spawnedEntity = createEntityFromTemplate(templateName, id);
		result = spawnedEntity.get();

		if (blockGrid && result)
		{
			if (auto transformComponent = result->findComponent<TransformComponent>())
			{
				transformComponent->setCurrentBlockGrid(blockGrid);
				transformComponent->setLocalPosition(position);
			}
		}

		spawnedEntity->initComponents();

		spawnedEntities.emplace(id, std::move(spawnedEntity));
	}

	return result;
}

const std::vector<std::unique_ptr<CelestialBody>>& Server::getCelestialBodies()
{
	return celestialBodies;
}

void Server::possesEntity(const std::string& entityName, Client* const client)
{
	if (client)
	{
		if (spawnedEntities.find(entityName) != spawnedEntities.end())
		{
			client->setPossesedEntity(spawnedEntities.at(entityName).get());
		}
	}
}
