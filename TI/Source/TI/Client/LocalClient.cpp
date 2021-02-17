#include "LocalClient.h"

#include <filesystem>

#include <TI/Client/ClientConnectionRequest.h>
#include <TI/Server/Server.h>
#include <TI/Application.h>
#include <TI/Client/Input/InputHandler.h>
#include <TI/Client/Controller.h>
#include <TI/Util/Config.h>
#include <TI/Server/Component/MeshComponent.h>
#include <TI/Server/Component/CameraComponent.h>
#include <TI/Server/Entity.h>
#include <TI/Renderer/Renderer.h>

LocalClient::LocalClient(Application* app, const std::string& name) :
	Client(app),
	viewportId(0)
{
	inputHandler = std::make_unique<InputHandler>();
	playerController = std::make_unique<PlayerController>(this, inputHandler.get());

	this->name = name.empty() ? DEFAULT_PLAYER_NAME : name;

	loadConfig();
	loadMappings();
}

void LocalClient::connect()
{
	Server* server = app->getCurrentServer();
	if (server)
	{
		server->connectClient({ this });
	}
}

void LocalClient::receiveServerConnectionResponse(ServerConnectionResponse response)
{
	id = response.id;
}

void LocalClient::possesEntity(Entity* entity)
{
	Client::possesEntity(entity);

	playerController->posses(entity);

	if (entity)
	{
		auto cameraComp = entity->findComponent<CameraComponent>();
		if (cameraComp)
		{
			if (app)
			{
				auto renderer = app->getRenderer();
				if (renderer)
				{
					auto viewport = renderer->getViewport(viewportId);
					if (viewport)
					{
						viewport->setCamera(cameraComp->getCamera());
					}
				}
			}
		}
	}
}

void LocalClient::update(float dt)
{
	// Render
	if (app)
	{
		Renderer* const renderer = app->getRenderer();
		if (renderer)
		{
			auto server = app->getCurrentServer();
			if (server)
			{
				for (auto& entityPair : server->getEntities())
				{
					auto& entity = entityPair.second;

					auto meshComp = entity->findComponent<MeshComponent>();
					if (meshComp)
					{
						if (entity->getName() != name)
						{
							renderer->pushRender(meshComp, viewportId);
						}
					}
				}
			}
		}
	}
}

InputHandler* const LocalClient::getInputHandler() const
{
	return inputHandler.get();
}

IController* const LocalClient::getController() const
{
	return playerController.get();
}

void LocalClient::shutdown()
{
	config.save();

	Client::shutdown();
}

void LocalClient::setViewportId(unsigned int id)
{
	viewportId = id;
}

unsigned int LocalClient::getViewportId() const
{
	return viewportId;
}

void LocalClient::loadConfig()
{
	std::string configFolderPath = CONFIG_FOLDER + std::string("Players/");
	std::string configFilePath = configFolderPath + name + CONFIG_EXTENSION;
	if (!std::filesystem::exists(configFolderPath))
	{
		std::filesystem::create_directories(configFolderPath);
	}

	if (!std::filesystem::exists(configFilePath))
	{
		std::filesystem::copy_file(CONFIG_FOLDER + std::string("Default.cfg"), configFilePath);
	}

	config.load(configFilePath);
}

void LocalClient::loadMappings()
{
	auto axisEntries = config.getAxisEntries();
	for (const auto& entry : axisEntries)
	{
		if (entry.axis == Axis::UnknownAxis)
		{
			inputHandler->addAxisMapping(entry.key, AxisMapping({ entry.bindingName, entry.scale }));
		}
		else
		{
			inputHandler->addAxisMapping(entry.axis, AxisMapping({ entry.bindingName, entry.scale }));
		}
	}

	auto keyEntries = config.getKeyEntries();
	for (const auto& entry : keyEntries)
	{
		inputHandler->addKeyMapping(entry.key, entry.bindingName);
	}
}
