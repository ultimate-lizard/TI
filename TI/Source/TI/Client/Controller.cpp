#include "Controller.h"

#include <iostream>

#include <SDL.h>
#include <glad/glad.h>

#include <TI/Client/Input/InputHandler.h>
#include <TI/Renderer/Camera.h>
#include <TI/Server/Component/MovementComponent.h>
#include <TI/Server/Component/TransformComponent.h>
#include <TI/Server/Component/CameraComponent.h>
#include <TI/Application.h>
#include <TI/Server/BlockGrid.h>
#include <TI/Renderer/Renderer.h>
#include <TI/Client/LocalClient.h>
#include <TI/Server/LocalServer.h>
#include <TI/Client/DebugInformation.h>

// A number to multiply camera sensivity during controller camera
// vertical and horizontal movements to match the sensivity of the mouse input
static const float CONTROLLER_SENSIVITY_ADJUSTER = 100.0f;

PlayerController::PlayerController(Client* client, InputHandler* inputHandler) :
	inputHandler(inputHandler),
	// entity(nullptr),
	movementComponent(nullptr),
	client(client),
	thirdperson(false),
	mouseLookSensivity(13.0f),
	controllerLookSensivity(2.5f)
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
		movementComponent->addVerticalLook(value * mouseLookSensivity);
	}
}

void PlayerController::handleLookHorizontal(float value)
{
	if (movementComponent)
	{
		movementComponent->addHorizontalLook(value * mouseLookSensivity);
	}
}

void PlayerController::handleLookVerticalRate(float value)
{
	if (movementComponent)
	{
		movementComponent->setPitchRate(value * CONTROLLER_SENSIVITY_ADJUSTER * controllerLookSensivity);
	}
}

void PlayerController::handleLookHorizontalRate(float value)
{
	if (movementComponent)
	{
		movementComponent->setYawRate(value * CONTROLLER_SENSIVITY_ADJUSTER * controllerLookSensivity);
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

//void PlayerController::castRayWithCollision()
//{
//	static int entityCount = 0;
//
//	auto playerPos = entity->findComponent<TransformComponent>()->getPosition();
//	auto playerForward = movementComponent->getHeadDirection();
//
//	auto spawnLocation = playerPos + playerForward * 1.5f;
//	auto spawnVelocity = playerForward * 50.0f;
//
//	auto projectile = client->getApplication()->getCurrentServer()->spawnEntity("Cube", "123" + std::to_string(entityCount++), spawnLocation);
//}

void PlayerController::destroyBlock()
{
	if (entity)
	{
		if (auto transformComponent = entity->findComponent<TransformComponent>())
		{
			if (BlockGrid* plane = transformComponent->getCurrentBlockGrid())
			{
				if (auto localClient = dynamic_cast<LocalClient*>(client))
				{
					if (auto movementComponent = entity->findComponent<MovementComponent>())
					{
						float distance = 3.0f;
						glm::vec3 start = movementComponent->getHeadPosition();

						for (float i = 0.0f; i < distance; i += 0.01f)
						{
							glm::uvec3 rayEndPosition = start + movementComponent->getHeadDirection() * i;
							if (plane->getBlock(rayEndPosition) != 0)
							{
								plane->spawnBlock(rayEndPosition, 0);

								localClient->updateBlock(plane, rayEndPosition);

								break;
							}
						}
					}
				}
			}
		}
	}
}

void PlayerController::placeBlock()
{
	if (entity)
	{
		if (auto physicsComponent = entity->findComponent<PhysicsComponent>())
		{
			if (movementComponent)
			{
				auto transformComponent = entity->findComponent<TransformComponent>();
				glm::vec3 headPosition = movementComponent->getHeadPosition();

				for (float i = 0; i < 5.0f; i += 0.1f)
				{
					if (BlockGrid* plane = transformComponent->getCurrentBlockGrid())
					{
						if (plane->getBlock(headPosition + movementComponent->getHeadDirection() * i) == 1)
						{
							std::optional<RayCollisionResult> raycastResult = physicsComponent->checkRayVsAabb(headPosition, movementComponent->getHeadDirection(), glm::uvec3(headPosition + movementComponent->getHeadDirection() * i), { { 1.0f, 1.0f, 1.0f }, {0.0f, 0.0f, 0.0f} });
							if (raycastResult.has_value())
							{
								glm::ivec3 blockPosition = headPosition + movementComponent->getHeadDirection() * raycastResult->near + raycastResult->normal * 0.5f;
								if (plane->getBlock(blockPosition) == 0)
								{
									if (auto physicsComponent = entity->findComponent<PhysicsComponent>())
									{
										glm::vec3 blockCenterPosition {blockPosition.x + 0.5f, blockPosition.y + 0.5f, blockPosition.z + 0.5f};

										if (!physicsComponent->checkCollision(blockCenterPosition, transformComponent->getPosition() + physicsComponent->getCollisionBox().getOffset(), {glm::vec3(1.0f), glm::vec3(0.0f)}, physicsComponent->getCollisionBox()))
										{
											plane->spawnBlock(blockPosition, 1);
											if (client)
											{
												if (auto localClient = dynamic_cast<LocalClient*>(client))
												{
													localClient->updateBlock(plane, blockPosition);
												}
											}
										}
									}
								}
							}

							break;
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
			if (BlockGrid* const plane = transformComponent->getCurrentBlockGrid())
			{
				plane->setGravityEnabled(!plane->isGravityEnabled());
			}
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
				cameraPosition.z = thirdperson ? -3.0f : 0.0f;
				camera->setPosition(cameraPosition);
			}
		}
	}
}

void PlayerController::toggleFrustumCulling()
{
	if (auto localClient = dynamic_cast<LocalClient*>(client))
	{
		localClient->setFrustumCullingEnabled(!localClient->isFrustumCullingEnabled());
	}
}

void PlayerController::jump()
{
	if (movementComponent)
	{
		movementComponent->jump();
	}
}

void PlayerController::increaseFlightSpeed()
{
	if (movementComponent)
	{
		movementComponent->setFlightSpeed(movementComponent->getFlightSpeed() * 2.0f);
	}
}

void PlayerController::decreaseFlightSpeed()
{
	if (movementComponent)
	{
		movementComponent->setFlightSpeed(movementComponent->getFlightSpeed() * 0.5f);
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

		inputHandler->bindKey("SpawnBlock", ActionInputType::Press, std::bind(&PlayerController::placeBlock, this));
		inputHandler->bindKey("DestroyBlock", ActionInputType::Press, std::bind(&PlayerController::destroyBlock, this));

		inputHandler->bindKey("TogglePolygonMode", ActionInputType::Press, std::bind(&PlayerController::togglePolygonMode, this));
		inputHandler->bindKey("ToggleFlyMode", ActionInputType::Press, std::bind(&PlayerController::toggleFlyMode, this));
		inputHandler->bindKey("ToggleCollisionInfo", ActionInputType::Press, std::bind(&PlayerController::toggleCollisionInfo, this));
		inputHandler->bindKey("ToggleThirdperson", ActionInputType::Press, std::bind(&PlayerController::toggleThirdperson, this));
		inputHandler->bindKey("ToggleFrustumCulling", ActionInputType::Press, std::bind(&PlayerController::toggleFrustumCulling, this));

		inputHandler->bindKey("Jump", ActionInputType::Press, std::bind(&PlayerController::jump, this));

		inputHandler->bindKey("IncreaseFlightSpeed", ActionInputType::Press, std::bind(&PlayerController::increaseFlightSpeed, this));
		inputHandler->bindKey("DecreaseFlightSpeed", ActionInputType::Press, std::bind(&PlayerController::decreaseFlightSpeed, this));
	}
}
