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
// #include <TI/Server/AstroBody.h>
#include <TI/Server/Star.h>
#include <TI/Server/Planet.h>

LocalServer::LocalServer(Application* app) :
	Server(app)
{
	initStarSystems();

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

	if (!stars.empty())
	{
		if (Star* star = stars[0].get())
		{
			auto& planets = star->getPlanets();
			if (!planets.empty())
			{
				if (Planet* planet = planets[0].get())
				{
					static float angle = 0;
					angle += 0.01f * dt;

					if (angle > 360.0f)
					{
						angle = 0;
					}

					const float radius = 750.0f;

					glm::vec3 newPosition(glm::cos(glm::radians(angle)) * radius, 0.0f, glm::sin(glm::radians(angle)) * radius);

					glm::vec3 offset = newPosition - planet->getLocalPosition(CoordinateSystem::Interplanetary);
					planet->offset(offset, CoordinateSystem::Interplanetary);
					planet->rotate(0.01f * dt, { 0.0f, 1.0f, 0.0f }, CoordinateSystem::Interplanetary);

					// TODO: For each player on the server
					if (Entity* player = findEntity("Player"))
					{
						if (auto playerTransform = player->findComponent<TransformComponent>())
						{
							if (playerTransform->isChildOf(planet, CoordinateSystem::Interplanetary))
							{
								const float playerToPlanetDist = glm::distance(playerTransform->getDerivedPosition(CoordinateSystem::Interplanetary), planet->getDerivedPosition(CoordinateSystem::Interplanetary));
								if (playerToPlanetDist > 2.0f)
								{
									playerTransform->setLocalPosition(playerTransform->getDerivedPosition(CoordinateSystem::Interplanetary), CoordinateSystem::Interplanetary);
									playerTransform->setLocalPosition(playerTransform->getDerivedPosition(CoordinateSystem::Interstellar), CoordinateSystem::Interstellar);
									/*playerTransform->setLocalOrientation(playerTransform->getDerivedOrientation(CoordinateSystem::Interplanetary), CoordinateSystem::Interplanetary);
									playerTransform->setLocalOrientation(playerTransform->getDerivedOrientation(CoordinateSystem::Interstellar), CoordinateSystem::Interstellar);*/
									playerTransform->removeParent();
								}
							}
						}
					}
				}
			}
		}
	}
}

bool LocalServer::connectClient(Client* client, const std::string& ip, int port)
{
	// Get home planet for spawn
	if (BlockGrid* bg = stars[0]->getPlanets()[0]->getBlockGrid())
	{
		glm::ivec3 planeSize = bg->getBlockGridSize();
		glm::vec3 spawnLocation{ planeSize.x * bg->getChunkSize() / 2.0f, planeSize.y * bg->getChunkSize() / 2.0f, 3.0f };

		spawnPlayer(client, stars[0]->getPlanets()[0].get(), spawnLocation);
	}

	return true;
}

void LocalServer::ejectClient(Client* client)
{
	spawnedEntities.erase(client->getName());
}

void LocalServer::initStarSystems()
{
	// planes.push_back(std::make_unique<Plane>(glm::vec3(0.0f), glm::uvec3(10), 16));
	stars.push_back(std::make_unique<Star>());
	stars[0]->initHomeStarSystem();

	const size_t numStars = 100;
	for (size_t i = 0; i < numStars; ++i)
	{
		auto newStar = std::make_unique<Star>();
		newStar->initRandomStar();
		stars.push_back(std::move(newStar));
	}
}

void LocalServer::spawnPlayer(Client* const client, Planet* planet, const glm::vec3& position)
{
	if (planet)
	{
		if (BlockGrid* blockGrid = planet->getBlockGrid())
		{
			// glm::ivec3 planeSize = plane->getSize();

			// glm::vec3 spawnLocation { planeSize.x * planes[0].get()->getChunkSize() / 2.0f, planeSize.y * planes[0].get()->getChunkSize() + 3.0f, planeSize.z * planes[0].get()->getChunkSize() / 2.0f};
			Entity* playerEntity = spawnEntity("Player", client->getName(), blockGrid, position);
			possesEntity(client->getName(), client);

			if (auto transformComponent = playerEntity->findComponent<TransformComponent>())
			{
				// TODO: This adjustment must be implicit
				transformComponent->setLocalPosition({ 0.0f, 0.0f, -0.085f }, CoordinateSystem::Interplanetary);

				transformComponent->setParent(planet, CoordinateSystem::Interplanetary);
				// transformComponent->setScale(transformComponent->getScale(CoordinateSystem::Interplanetary) * 1212312313.001f, CoordinateSystem::Interplanetary);
			}
		}
	}
}
