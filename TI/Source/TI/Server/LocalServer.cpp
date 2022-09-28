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
	initHomeSolarSystem();

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

	for (const std::unique_ptr<Star>& star : starSystems)
	{
		if (star)
		{
			star->tick(dt);
		}
	}

	for (const std::unique_ptr<CelestialBody>& body : celestialBodies)
	{
		if (body)
		{
			body->tick(dt);
		}
	}

	for (auto& entityPair : spawnedEntities)
	{
		entityPair.second->tick(dt);
	}
}

bool LocalServer::connectClient(Client* client, const std::string& ip, int port)
{
	if (homePlanet)
	{
		if (BlockGrid* bg = homePlanet->getBlockGrid())
		{
			glm::ivec3 planeSize = bg->getBlockGridSize();
			glm::vec3 spawnLocation{ planeSize.x * bg->getChunkSize() / 2.0f, planeSize.y * bg->getChunkSize() + 3.0f, planeSize.z * bg->getChunkSize() / 2.0f };
			spawnPlayer(client, nullptr, spawnLocation);
		}
	}
	else
	{
		spawnPlayer(client, nullptr, {});
	}

	return true;
}

void LocalServer::ejectClient(Client* client)
{
	spawnedEntities.erase(client->getName());
}

void LocalServer::initHomeSolarSystem()
{
	auto star = std::make_unique<Star>();
	star->setLocalScale(glm::vec3(100.0f), CoordinateSystem::Interplanetary);
	// star->setLocalScale(glm::vec3(0.100f), CoordinateSystem::Interstellar);

	auto blockGrid = std::make_unique<BlockGrid>(glm::uvec3(10), 16);
	auto planet = std::make_unique<Planet>(blockGrid.get());

	planet->setLocalScale(glm::vec3(0.1f), CoordinateSystem::Interplanetary);

	OrbitalProperties planetProperties;
	planetProperties.radius = 750.0f;
	planetProperties.orbitalVelocity = 0.1f;
	planetProperties.equatorialVelocity = 0.5f;

	planet->setOrbitalProperties(std::move(planetProperties));
	homePlanet = planet.get();

	// --- Sattelite ---

	auto sattelite = std::make_unique<Planet>();

	sattelite->setLocalScale(glm::vec3(0.05f), CoordinateSystem::Interplanetary);

	OrbitalProperties satteliteProperties;

	satteliteProperties.radius = 2.0f;
	satteliteProperties.orbitalVelocity = 10.0f;
	satteliteProperties.equatorialVelocity = 2.5f;
	sattelite->setOrbitalProperties(std::move(satteliteProperties));

	// -----------------

	planet->addSattelite(sattelite.get());
	star->addSattelite(planet.get());
	
	blockGrids.push_back(std::move(blockGrid));
	starSystems.push_back(std::move(star));
	celestialBodies.push_back(std::move(sattelite));
	celestialBodies.push_back(std::move(planet));
}

void LocalServer::spawnPlayer(Client* const client, Planet* planet, const glm::vec3& position)
{
	BlockGrid* blockGrid = nullptr;

	if (planet)
	{
		blockGrid = planet->getBlockGrid();
	}

	Entity* playerEntity = spawnEntity("Player", client->getName(), blockGrid, position);
	possesEntity(client->getName(), client);

	if (auto transformComponent = playerEntity->findComponent<TransformComponent>())
	{
		// TODO: Find a better way to init the missing node
		transformComponent->leaveNode(CoordinateSystem::Planetary);
		transformComponent->setLocalPosition({ 760.0f, 0.0f, 0.0f }, CoordinateSystem::Interplanetary);
		// TODO: This adjustment must be implicit
		//transformComponent->setLocalPosition({ 0.0f, 0.085f, 0.0f }, CoordinateSystem::Interplanetary);
		//transformComponent->setParent(planet, CoordinateSystem::Interplanetary);
		//transformComponent->setPrimaryBody(planet);
	}
}
