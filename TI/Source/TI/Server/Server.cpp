#include "Server.h"

#include <memory>
#include <stdexcept>

#include <TI/Server/Component/MeshComponent.h>
#include <TI/Server/Component/MovementComponent.h>
#include <TI/Server/Component/CameraComponent.h>
#include <TI/Server/Component/TransformComponent.h>
#include <TI/Server/Component/PhysicsComponent.h>
#include <TI/Application.h>
#include <TI/Client/LocalClient.h>
#include <TI/Renderer/Camera.h>
#include <TI/Server/Plane.h>

void Server::initEntityTemplates()
{
	auto playerEntity = std::make_unique<Entity>();
	playerEntity->setName("Player");

	auto transformComponent = playerEntity->addComponent<TransformComponent>();

	//auto meshComponent = playerEntity->addComponent<MeshComponent>(app->getResourceManager());
	/*meshComponent->setParent(transformComponent);
	meshComponent->loadModel("Player");
	meshComponent->setScale({ 0.3f, 0.3f, 0.3f });*/

	auto physicsComponent = playerEntity->addComponent<PhysicsComponent>();
	physicsComponent->setCollisionBox({ { -0.2f, -1.9f, -0.2f }, { 0.2f, 0.0f, 0.2f } });

	auto movementComponent = playerEntity->addComponent<MovementComponent>();

	auto cameraComponent = playerEntity->addComponent<CameraComponent>();
	auto camera = std::make_unique<Camera>();
	camera->setRotation({ 0.0f, 90.0f, 0.0f });
	camera->setPosition({ -0.0f, 0.0f, -0.0f });
	camera->setParent(cameraComponent);
	cameraComponent->setCamera(std::move(camera));
	cameraComponent->setParent(transformComponent);
	// assert(transformComponent);
	// assert(meshComponent);
	// assert(physicsComponent);
	// assert(cameraComponent);
	// assert(movementComponent);

	entityTemplates.emplace(playerEntity->getName(), std::move(playerEntity));

	auto cubeEntity = std::make_unique<Entity>();
	cubeEntity->setName("Cube");
	auto cubeTransformComponent = cubeEntity->addComponent<TransformComponent>();
	auto cubeMeshComponent = cubeEntity->addComponent<MeshComponent>(app->getResourceManager());
	cubeMeshComponent->setParent(cubeTransformComponent);
	cubeMeshComponent->loadModel("Cube");
	auto cubePhysicsComponent = cubeEntity->addComponent<PhysicsComponent>();
	cubePhysicsComponent->setCollisionBox({ { -0.5f, -0.5, -0.5f },  { 0.5f, 0.5, 0.5f } });

	entityTemplates.emplace(cubeEntity->getName(), std::move(cubeEntity));
}

Server::Server(Application* app) :
	app(app), shuttingDown(false), plane(nullptr)
{
	plane = new Plane(glm::vec3(1.0f), { 4, 1, 4 });
}

Server::~Server()
{
	if (plane)
	{
		delete plane;
	}
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

const Plane* const Server::getPlane() const
{
	return plane;
}

Entity* const Server::spawnEntity(const std::string& templateName, const std::string& id, const glm::vec3& position)
{
	Entity* result = nullptr;

	if (!findEntity(id))
	{
		std::unique_ptr<Entity> spawnedEntity = createEntityFromTemplate(templateName, id);
		result = spawnedEntity.get();

		if (plane)
		{
			if (auto transformComponent = result->findComponent<TransformComponent>())
			{
				transformComponent->setPlane(plane);
				transformComponent->setPosition(position);
			}
		}

		spawnedEntity->initComponents();

		spawnedEntities.emplace(id, std::move(spawnedEntity));
	}

	return result;
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
