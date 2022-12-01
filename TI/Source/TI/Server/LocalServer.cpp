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

	for (size_t i = 0; i < 100; ++i)
	{
		initRandomSolarSystem();
	}

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

	for (const std::unique_ptr<CelestialBody>& star : starSystems)
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
	spawnPlayer(client, nullptr, {});

	return true;
}

void LocalServer::ejectClient(Client* client)
{
	spawnedEntities.erase(client->getName());
}

CelestialBody* LocalServer::findClosestCelestialBody(CoordinateSystem cs, const glm::vec3& position)
{
	const std::vector<std::unique_ptr<CelestialBody>>* bodiesVector = nullptr;

	switch (cs)
	{
	case CoordinateSystem::Interstellar:
		bodiesVector = &starSystems;
		break;

	case CoordinateSystem::Interplanetary:
		bodiesVector = &celestialBodies;
		break;

	// TODO: Intergalactical

	default:
		return nullptr;
	}

	float minDistance = std::numeric_limits<float>::max();
	CelestialBody* closestBody = nullptr;

	for (auto& body : *bodiesVector)
	{
		float distance = glm::distance(position, body->getDerivedPosition(cs, false));

		if (distance <= minDistance)
		{
			minDistance = distance;
			closestBody = body.get();
		}
	}

	return closestBody;
}

void LocalServer::initHomeSolarSystem()
{
	auto star = std::make_unique<Star>();
	star->setLocalScale(glm::vec3(0.1f), CoordinateSystem::Interstellar);

	auto planetBg = std::make_unique<BlockGrid>(glm::uvec3(10), 16);
	auto planet = std::make_unique<Planet>(planetBg.get());

	planet->setLocalScale(glm::vec3(0.1f), CoordinateSystem::Interplanetary);

	OrbitalProperties planetProperties;
	planetProperties.radius = 750.0f;
	planetProperties.orbitalVelocity = 0.1f;
	planetProperties.equatorialVelocity = 0.5f;

	planet->setOrbitalProperties(std::move(planetProperties));

	// --- Sattelite ---

	auto satteliteBg = std::make_unique<BlockGrid>(glm::uvec3(10), 16);
	auto sattelite = std::make_unique<Planet>(satteliteBg.get());

	sattelite->setLocalScale(glm::vec3(0.1f), CoordinateSystem::Interplanetary);

	OrbitalProperties satteliteProperties;

	satteliteProperties.radius = 2.0f;
	satteliteProperties.orbitalVelocity = 2.0f;
	satteliteProperties.equatorialVelocity = 2.5f;
	sattelite->setOrbitalProperties(std::move(satteliteProperties));

	// -----------------

	planet->addSattelite(sattelite.get());
	star->addSattelite(planet.get());
	
	blockGrids.push_back(std::move(planetBg));
	blockGrids.push_back(std::move(satteliteBg));
	starSystems.push_back(std::move(star));
	celestialBodies.push_back(std::move(sattelite));
	celestialBodies.push_back(std::move(planet));
}

void LocalServer::initRandomSolarSystem()
{
	auto star = std::make_unique<Star>();

	star->setLocalScale(glm::vec3(0.100f), CoordinateSystem::Interstellar); // 100 km in size

	const float low = -100.0f;
	const float high = 100.0f;

	float x = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
	float y = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
	float z = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));

	star->setLocalPosition({ x, y, z }, CoordinateSystem::Interstellar);

	// --- Planet ---

	auto planet = std::make_unique<Planet>();

	planet->setLocalScale(glm::vec3(20.0f), CoordinateSystem::Interplanetary);

	OrbitalProperties planetProperties;
	planetProperties.radius = 750.0f;
	planetProperties.orbitalVelocity = 1.0f;
	planetProperties.equatorialVelocity = 1.5f;

	planet->setOrbitalProperties(std::move(planetProperties));

	star->addSattelite(planet.get());

	starSystems.push_back(std::move(star));
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
		transformComponent->setLocalPosition({ 760.0f, 0.0f, 0.0f }, CoordinateSystem::Interplanetary);
	}
}
