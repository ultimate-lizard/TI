#include "Controller.h"

#include <iostream>

#include <SDL.h>
#include <glad/glad.h>

#include <TI/Client/Input/InputHandler.h>
#include <TI/Renderer/Camera.h>
#include <TI/Server/Component/MovementComponent.h>
#include <TI/Server/Component/TransformComponent.h>
#include <TI/Client/Client.h>
#include <TI/Client/LocalClient.h>
#include <TI/Application.h>
#include <TI/Server/Plane.h>
#include <TI/Client/ChunkMesh.h>
#include <TI/Renderer/Renderer.h>

// A number to multiply camera sensivity during controller camera
// vertical and horizontal movements to match the sensivity of the mouse input
static const float CONTROLLER_SENSIVITY_ADJUSTER = 100.0f;

PlayerController::PlayerController(Client* client, InputHandler* inputHandler) :
	inputHandler(inputHandler),
	// entity(nullptr),
	movementComp(nullptr),
	client(client)
{
	setupInputHandler();
}

void PlayerController::handleMovementForward(float value)
{
	if (movementComp)
	{
		movementComp->setMovementForward(value);
	}
}

void PlayerController::handleMovementSideways(float value)
{
	if (movementComp)
	{
		movementComp->setMovementSideways(value);
	}
}

void PlayerController::handleLookVertical(float value)
{
	if (movementComp)
	{
		movementComp->addVerticalLook(value);
	}
}

void PlayerController::handleLookHorizontal(float value)
{
	if (movementComp)
	{
		movementComp->addHorizontalLook(value);
	}
}

void PlayerController::handleLookVerticalRate(float value)
{
	if (movementComp)
	{
		movementComp->setPitchRate(value * CONTROLLER_SENSIVITY_ADJUSTER);
	}
}

void PlayerController::handleLookHorizontalRate(float value)
{
	if (movementComp)
	{
		movementComp->setYawRate(value * CONTROLLER_SENSIVITY_ADJUSTER);
	}
}


void PlayerController::posses(Entity* entity)
{
	this->entity = entity;
	if (entity)
	{
		movementComp = entity->findComponent<MovementComponent>();
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
						const float COLLISION_PRECISION = 0.01f;

						glm::vec3 start = transformComponent->getPosition();

						float distance = 10.0f;
						glm::vec3 end = start + movementComponent->getForward() * distance;

						bool found = false;

						for (float i = 0.0f; i < distance; i += COLLISION_PRECISION)
						{
							glm::vec3 cur = start + movementComponent->getForward() * i;

							if (plane->getBlock(cur) != 0)
							{
								localClient->drawDebugPoint(cur, { 1.0f, 0.0f, 0.0f, 1.0f }, 20.0f);
								localClient->drawDebugLine(start, cur, { 0.0f, 1.0f, 0.0f, 1.0f }, 2.0f);

								found = true;

								break;
							}
						}

						if (!found)
						{
							localClient->drawDebugLine(start, end, { 0.0f, 1.0f, 0.0f, 1.0f }, 2.0f);
						}
					}
				}
			}			
		}
	}
}

void PlayerController::destroyBlock()
{
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
							glm::vec3 cur = start + movementComponent->getForward() * i;
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
			renderer->setPolygonMode(polygon ? GL_LINE : GL_FILL);
		}
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

		inputHandler->bindKey("QuitGame", ActionInputType::KeyPress, std::bind(&PlayerController::quitGame, this));
		inputHandler->bindKey("ReleaseMouse", ActionInputType::KeyPress, std::bind(&PlayerController::releaseMouse, this));

		inputHandler->bindKey("SpawnBlock", ActionInputType::KeyPress, std::bind(&PlayerController::castRayWithCollision, this));
		inputHandler->bindKey("DestroyBlock", ActionInputType::KeyPress, std::bind(&PlayerController::destroyBlock, this));

		inputHandler->bindKey("TogglePolygonMode", ActionInputType::KeyPress, std::bind(&PlayerController::togglePolygonMode, this));
	}
}
