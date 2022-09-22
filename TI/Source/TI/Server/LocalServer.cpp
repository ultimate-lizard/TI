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
#include <TI/Server/SolarSystem.h>
#include <TI/Server/PlanetarySystem.h>

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

	for (auto& entityPair : spawnedEntities)
	{
		entityPair.second->tick(dt);
	}

	for (const std::unique_ptr<OrbitalSystem>& solarSystem : orbitalSystems)
	{
		if (solarSystem)
		{
			solarSystem->tick(dt);
		}
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
			spawnPlayer(client, homePlanet, spawnLocation);
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
	star->setLocalScale(glm::vec3(0.100f), CoordinateSystem::Interstellar);

	auto blockGrid = std::make_unique<BlockGrid>(glm::uvec3(10), 16);
	auto planet = std::make_unique<Planet>(blockGrid.get());

	planet->setLocalScale(glm::vec3(0.08f), CoordinateSystem::Interplanetary);
	// planet->rotate(25.0f, { 0.0f, 0.0f, 1.0f }, CoordinateSystem::Interplanetary);

	OrbitalProperties properties;
	properties.radius = 750.0f;
	properties.orbitalVelocity = 0.0f;
	properties.equatorialVelocity = 0.0f;

	planet->setOrbitalProperties(std::move(properties));

	homePlanet = planet.get();

	auto solarSystem = std::make_unique<SolarSystem>(star.get());
	auto planetarySystem = std::make_unique<PlanetarySystem>(planet.get());
	solarSystem->addOrbitalSystem(planetarySystem.get());
	
	blockGrids.push_back(std::move(blockGrid));
	celestialBodies.push_back(std::move(star));
	celestialBodies.push_back(std::move(planet));
	orbitalSystems.push_back(std::move(solarSystem));
	orbitalSystems.push_back(std::move(planetarySystem));

	//const size_t numStars = 150;
	//for (size_t i = 0; i < numStars; ++i)
	//{
	//	auto newStar = std::make_unique<Star>();
	//	newStar->initRandomStar();
	//	celestialBodies.push_back(std::move(newStar));
	//}
}

void LocalServer::spawnPlayer(Client* const client, Planet* planet, const glm::vec3& position)
{
	if (planet)
	{
		if (BlockGrid* blockGrid = planet->getBlockGrid())
		{
			Entity* playerEntity = spawnEntity("Player", client->getName(), blockGrid, position);
			possesEntity(client->getName(), client);

			if (auto transformComponent = playerEntity->findComponent<TransformComponent>())
			{
				// TODO: This adjustment must be implicit
				transformComponent->setLocalPosition({ 0.0f, 0.085f, 0.0f }, CoordinateSystem::Interplanetary);
				transformComponent->setParent(planet, CoordinateSystem::Planetary);
			}
		}
	}
}
