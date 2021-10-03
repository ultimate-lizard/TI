#include "Controller.h"

#include <iostream>

#include <SDL.h>
#include <glad/glad.h>

#include <TI/Client/Input/InputHandler.h>
#include <TI/Renderer/Camera.h>
#include <TI/Server/Component/MovementComponent.h>
#include <TI/Server/Component/TransformComponent.h>
#include <TI/Server/Component/CameraComponent.h>
#include <TI/Client/Client.h>
#include <TI/Application.h>
#include <TI/Server/Plane.h>
#include <TI/Client/ChunkMesh.h>
#include <TI/Renderer/Renderer.h>
#include <TI/Client/DebugInformation.h>
#include <TI/Client/LocalClient.h>
#include <TI/Server/LocalServer.h>

// A number to multiply camera sensivity during controller camera
// vertical and horizontal movements to match the sensivity of the mouse input
static const float CONTROLLER_SENSIVITY_ADJUSTER = 100.0f;

PlayerController::PlayerController(Client* client, InputHandler* inputHandler) :
	inputHandler(inputHandler),
	// entity(nullptr),
	movementComponent(nullptr),
	client(client),
	thirdperson(false)
{
	setupInputHandler();
}

void PlayerController::handleMovementForward(float value)
{
	if (movementComponent)
	{
		movementComponent->setMovementForward(value);
	}
}

void PlayerController::handleMovementSideways(float value)
{
	if (movementComponent)
	{
		movementComponent->setMovementSideways(value);
	}
}

void PlayerController::handleLookVertical(float value)
{
	if (movementComponent)
	{
		movementComponent->addVerticalLook(value);
	}
}

void PlayerController::handleLookHorizontal(float value)
{
	if (movementComponent)
	{
		movementComponent->addHorizontalLook(value);
	}
}

void PlayerController::handleLookVerticalRate(float value)
{
	if (movementComponent)
	{
		movementComponent->setPitchRate(value * CONTROLLER_SENSIVITY_ADJUSTER);
	}
}

void PlayerController::handleLookHorizontalRate(float value)
{
	if (movementComponent)
	{
		movementComponent->setYawRate(value * CONTROLLER_SENSIVITY_ADJUSTER);
	}
}


void PlayerController::posses(Entity* entity)
{
	this->entity = entity;
	if (entity)
	{
		movementComponent = entity->findComponent<MovementComponent>();
	}
}

void PlayerController::quitGame()
{
	auto app = client->getApplication();
	if (app)
	{
		app->requestQuit();
	}
}

void PlayerController::releaseMouse()
{
	auto app = client->getApplication();
	app->getInput()->releaseMouse();
}

void PlayerController::castRayWithCollision()
{
	static int entityCount = 0;

	auto playerPos = entity->findComponent<TransformComponent>()->getPosition();
	auto playerForward = movementComponent->getHeadDirection();

	auto spawnLocation = playerPos + playerForward * 1.5f;
	auto spawnVelocity = playerForward * 50.0f;

	auto projectile = client->getApplication()->getCurrentServer()->spawnEntity("Cube", "123" + std::to_string(entityCount++), spawnLocation);

	/*if (entity)
	{
		if (auto transformComponent = entity->findComponent<TransformComponent>())
		{
			if (Plane* plane = transformComponent->getPlane())
			{
				if (auto localClient = dynamic_cast<LocalClient*>(client))
				{
					if (auto movementComponent = entity->findComponent<MovementComponent>())
					{
						const float COLLISION_PRECISION = 0.01f;

						glm::vec3 start = transformComponent->getPosition();

						float distance = 10.0f;
						glm::vec3 end = start + movementComponent->getHeadForward() * distance;

						bool found = false;

						for (float i = 0.0f; i < distance; i += COLLISION_PRECISION)
						{
							glm::vec3 cur = start + movementComponent->getHeadForward() * i;

							if (plane->getBlock(cur) != 0)
							{
								drawDebugPoint(cur, { 1.0f, 0.0f, 0.0f, 1.0f }, 20.0f);
								drawDebugLine(start, cur, { 0.0f, 1.0f, 0.0f, 1.0f }, 2.0f);

								found = true;

								break;
							}
						}

						if (!found)
						{
							drawDebugLine(start, end, { 0.0f, 1.0f, 0.0f, 1.0f }, 2.0f);
						}
					}
				}
			}			
		}
	}*/
}

void PlayerController::destroyBlock()
{
	SDL_Delay(400);
	if (entity)
	{
		if (auto transformComponent = entity->findComponent<TransformComponent>())
		{
			if (Plane* plane = transformComponent->getPlane())
			{
				if (auto localClient = dynamic_cast<LocalClient*>(client))
				{
					if (auto movementComponent = entity->findComponent<MovementComponent>())
					{
						float distance = 3.0f;
						glm::vec3 start = transformComponent->getPosition();

						for (float i = 0.0f; i < distance; i += 0.01f)
						{
							glm::vec3 cur = start + movementComponent->getHeadDirection() * i;
							if (plane->getBlock(cur) != 0)
							{
								plane->spawnBlock(cur, 0);

								/*unsigned int chunkIndex = plane->planePositionToChunkIndex(cur);
								localClient->getChunkMeshes()[chunkIndex]->rebuildMesh();*/

								for (ChunkMesh* mesh : localClient->getChunkMeshes())
								{
									mesh->rebuildMesh();
								}

								break;
							}
						}
					}
				}
			}
		}
	}
}

void PlayerController::togglePolygonMode()
{
	static bool polygon = false;

	if (client)
	{
		if (Renderer* renderer = client->getApplication()->getRenderer())
		{
			polygon = !polygon;
			renderer->setWireframe(polygon);
		}
	}
}

void PlayerController::toggleFlyMode()
{
	if (entity)
	{
		if (auto transformComponent = entity->findComponent<TransformComponent>())
		{
			Plane* const plane = transformComponent->getPlane();
			plane->setGravityEnabled(!plane->isGravityEnabled());
		}

		if (movementComponent)
		{
			movementComponent->setFlyModeEnabled(!movementComponent->isFlightModeEnabled());
		}
	}
}

void PlayerController::toggleCollisionInfo()
{
	if (client)
	{
		const std::map<std::string, std::unique_ptr<Entity>>& entities = client->getApplication()->getCurrentServer()->getEntities();
		for (const auto& pair : entities)
		{
			if (PhysicsComponent* physicsComponent = pair.second->findComponent<PhysicsComponent>())
			{
				physicsComponent->setRenderCollisionBoxEnabled(!physicsComponent->isRenderCollisiobBoxEnabled());
				physicsComponent->setRenderCollisions(!physicsComponent->isRenderCollisionsEnabled());
			}
		}
	}
}

void PlayerController::toggleThirdperson()
{
	thirdperson = !thirdperson;

	if (entity)
	{
		if (CameraComponent* cameraComponent = entity->findComponent<CameraComponent>())
		{
			if (Camera* camera = cameraComponent->getCamera())
			{
				glm::vec3 cameraPosition = camera->getPosition();
				cameraPosition.x = thirdperson ? -3.0f : 0.0f;
				camera->setPosition(cameraPosition);
			}
		}
	}
}

void PlayerController::jump()
{
	if (movementComponent)
	{
		movementComponent->jump();
	}
}

void PlayerController::setupInputHandler()
{
	if (inputHandler)
	{
		inputHandler->bindAxis("ForwardMovementAxis", std::bind(&PlayerController::handleMovementForward, this, std::placeholders::_1));
		inputHandler->bindAxis("SideMovementAxis", std::bind(&PlayerController::handleMovementSideways, this, std::placeholders::_1));

		inputHandler->bindAxis("HorizontalLook", std::bind(&PlayerController::handleLookHorizontal, this, std::placeholders::_1));
		inputHandler->bindAxis("VerticalLook", std::bind(&PlayerController::handleLookVertical, this, std::placeholders::_1));

		inputHandler->bindAxis("HorizontalLookRate", std::bind(&PlayerController::handleLookHorizontalRate, this, std::placeholders::_1));
		inputHandler->bindAxis("VerticalLookRate", std::bind(&PlayerController::handleLookVerticalRate, this, std::placeholders::_1));

		inputHandler->bindKey("QuitGame", ActionInputType::Press, std::bind(&PlayerController::quitGame, this));
		inputHandler->bindKey("ReleaseMouse", ActionInputType::Press, std::bind(&PlayerController::releaseMouse, this));

		inputHandler->bindKey("SpawnBlock", ActionInputType::Press, std::bind(&PlayerController::castRayWithCollision, this));
		inputHandler->bindKey("DestroyBlock", ActionInputType::Press, std::bind(&PlayerController::destroyBlock, this));

		inputHandler->bindKey("TogglePolygonMode", ActionInputType::Press, std::bind(&PlayerController::togglePolygonMode, this));
		inputHandler->bindKey("ToggleFlyMode", ActionInputType::Press, std::bind(&PlayerController::toggleFlyMode, this));
		inputHandler->bindKey("ToggleCollisionInfo", ActionInputType::Press, std::bind(&PlayerController::toggleCollisionInfo, this));
		inputHandler->bindKey("ToggleThirdperson", ActionInputType::Press, std::bind(&PlayerController::toggleThirdperson, this));

		inputHandler->bindKey("Jump", ActionInputType::Press, std::bind(&PlayerController::jump, this));
	}
}
